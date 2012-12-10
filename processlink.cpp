#include <cstdint>
#include <cstring>

#include <mutex>
#include <sstream>

#include "processlink.h"

#include "exceptions.h"
#include "fullslot.h"
#include "posixshmistream.h"
#include "posixshmostream.h"
#include "simpleslot.h"
#include "slottypes.h"
#include "typespec.h"

using namespace std;
using namespace glstreamer;

namespace
{
    struct CtrlBlock
    {
        uint32_t typeNameLength;
        uint32_t buffers;
        uint32_t bufferSize;
        char     typeName[];
    };
    struct Buffer
    {
        uint32_t size;
        char     data[];
    };
}

// Check list for link classes:
// DONE: Double link check
// DONE: Type matching
// DONE: Argument object assign and check
// DONE: Link reflection set and optionally check
// DONE: Slot pointer check

ProcessLink::ProcessLink ( const OutputSlot& srcSlot, const string& name, size_type queueSize ):
typeSpec(srcSlot.toSlot().typeSpec),
slot(&srcSlot.toSlot()),
emptyLeft(),
fullLeft(),
ctrlBlock(),
buffers(new PosixShm[queueSize]),
currentBufferId(0),
queueSize(queueSize),
arg(typeSpec->create(), Destroyer(typeSpec)),
direction(Output)
{
    openResources(name);
}

ProcessLink::ProcessLink ( const string& name, const InputSlot& dstSlot, size_type queueSize ):
typeSpec(dstSlot.toSlot().typeSpec),
slot(&dstSlot.toSlot()),
emptyLeft(),
fullLeft(),
ctrlBlock(),
buffers(new PosixShm[queueSize]),
currentBufferId(0),
queueSize(queueSize),
arg(typeSpec->create(), Destroyer(typeSpec)),
direction(Input)
{
    openResources(name);
}

ProcessLink::~ProcessLink() noexcept
{
    slot->simpleSlot->arg = nullptr;
    slot->simpleSlot->link = nullptr;
}

void ProcessLink::push ( SimpleSlot* src )
{
    if(direction != Output)
        throw UnsupportedOperation("push on non-output link");
    
    if(src != slot->simpleSlot)
        throw InternalError("slot doesn't match with link");
    
    unique_lock<PosixSem> emptyLocker(emptyLeft);
    
    CtrlBlock *ctrlBlockPtr = static_cast<CtrlBlock*>(ctrlBlock.map());
    if(ctrlBlockPtr->bufferSize != 0)
    {
        Buffer *buffer = static_cast<Buffer*>(buffers[currentBufferId].map());
        typeSpec->serialize_fixed(src->arg, buffer->data);
    }
    else
    {
        PosixShmOStream os(buffers[currentBufferId]);
        typeSpec->serialize_variable(src->arg, os);
    }
    
    fullLeft.post();
    currentBufferId = (currentBufferId + 1) % queueSize;
    
    emptyLocker.release();
}

void ProcessLink::fetch ( SimpleSlot* dst )
{
    if(direction != Input)
        throw UnsupportedOperation("fetch on non-input link");
    
    if(dst != slot->simpleSlot)
        throw InternalError("slot doesn't match with link");
    
    unique_lock<PosixSem> fullLocker(fullLeft);
    
    CtrlBlock *ctrlBlockPtr = static_cast<CtrlBlock*>(ctrlBlock.map());
    if(ctrlBlockPtr->bufferSize != 0)
    {
        Buffer *buffer = static_cast<Buffer*>(buffers[currentBufferId].map());
        typeSpec->deserialize_fixed(dst->arg, buffer->data);
    }
    else
    {
        PosixShmIStream is(buffers[currentBufferId]);
        typeSpec->deserialize_varialbe(dst->arg, is);
    }
    
    emptyLeft.post();
    currentBufferId = (currentBufferId + 1) % queueSize;
    
    fullLocker.release();
}

static inline string makeBufferName(string const& baseName, size_type index)
{
    ostringstream bufferName;
    bufferName << baseName << ".buf." << index;
    return bufferName.str();
}

void ProcessLink::openResources ( const string& name )
{
    if(slot->simpleSlot->link != nullptr)
        throw invalid_argument("Double link detected. Slot " + slot->name + " is already linked.");
    
    if(queueSize == 0)
        throw invalid_argument("Cannot allocate zero-sized queue.");
    
    string initName  = name + ".init";
    string emptyName = name + ".empty";
    string fullName  = name + ".full";
    string ctrlName  = name + ".ctrl";
    
    PosixSem initSem(initName, true, 0);
    if(initSem.hasOwnership())
    {
        emptyLeft = PosixSem(emptyName, true, queueSize);
        if(!emptyLeft.hasOwnership())
            throw ResourceConflict("semaphore " + emptyName);
        
        fullLeft = PosixSem(fullName, true, 0);
        if(!fullLeft.hasOwnership())
            throw ResourceConflict("semaphore " + fullName);
        
        string const typeName = typeSpec->id().name();
        
        ctrlBlock = PosixShm(ctrlName, true);
        if(!ctrlBlock.hasOwnership())
            throw ResourceConflict("shared memory: " + ctrlName);
        
        size_type ctrlBlockSize = sizeof(uint32_t) * 3 + typeName.length();
        ctrlBlock.resize(ctrlBlockSize);
        CtrlBlock* ctrlBlockPtr = static_cast<CtrlBlock*>(ctrlBlock.map(ctrlBlockSize));
        
        size_type bufferDataSize = typeSpec->serialize_size();
        ctrlBlockPtr->typeNameLength = typeName.length();
        ctrlBlockPtr->buffers = queueSize;
        ctrlBlockPtr->bufferSize = bufferDataSize;
        memcpy(ctrlBlockPtr->typeName, typeName.data(), typeName.length());
        
        for(size_type i = 0; i < queueSize; ++i)
        {
            string const bufferName = makeBufferName(name, i);
            buffers[i] = PosixShm(bufferName, true);
            if(!buffers[i].hasOwnership())
                throw ResourceConflict("shared memory: " + bufferName);
            
            size_type bufferInstanceSize = bufferDataSize + sizeof(uint32_t);
            buffers[i].resize(bufferInstanceSize);
            Buffer *buffer = static_cast<Buffer*>(buffers[i].map(bufferInstanceSize));
            buffer->size = bufferDataSize;
        }
        
        initSem.post();
        fullLeft.wait();
    }
    else
    {
        initSem = PosixSem(initName);
        initSem.wait();
        
        emptyLeft = PosixSem(emptyName);
        fullLeft = PosixSem(fullName);
        ctrlBlock = PosixShm(ctrlName);
        for(size_type i = 0; i < queueSize; ++i)
            buffers[i] = PosixShm(makeBufferName(name, i));
        
        CtrlBlock *ctrlBlockPtr = static_cast<CtrlBlock*>(ctrlBlock.map());
        size_type ctrlBlockSize = ctrlBlock.mappedSize();
        if(ctrlBlockSize != sizeof(uint32_t) * 3 + ctrlBlockPtr->typeNameLength)
            throw InternalError("control block has invalid size");
        
        string const typeName(ctrlBlockPtr->typeName, ctrlBlockPtr->typeName + ctrlBlockPtr->typeNameLength);
        if(typeName != typeSpec->id().name())
            throw invalid_argument(string("Types don't match: ") + typeSpec->id().name() + " against " + typeName);
        
        if(ctrlBlockPtr->buffers != queueSize)
            throw invalid_argument("Different queue size specified.");
        
        if(ctrlBlockPtr->bufferSize != typeSpec->serialize_size())
            throw invalid_argument("Serialize policy is not compatible.");
        
        fullLeft.post();
    }
    slot->simpleSlot->link = this;
    slot->simpleSlot->arg = arg.get();
}
