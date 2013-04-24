#include <stdexcept>

#include "SocketLinkOutput.h"

#include "exceptions.h"
#include "fullslot.h"
#include "MemBufferOStream.h"
#include "simpleslot.h"
#include "slottypes.h"
#include "typespec.h"

using namespace glstreamer;

// Check list for link classes:
// DONE: Double link check
// DONE: Type matching
// DONE: Argument object assign and check
// DONE: Link reflection set and optionally check
// DONE: Slot pointer check
// NOTE: Type matching is done on link. No exception is thrown. To be improved.

SocketLinkOutput::SocketLinkOutput ( const OutputSlot& srcSlot, const sockaddr* addr, socklen_t addrLen, Word32 linkId, size_type bufferCount ):
slot(&srcSlot.toSlot()),
typeSpec(slot->typeSpec),
fixedLen(typeSpec->serialize_size()),
arg(nullptr, Destroyer(typeSpec)),
linkBuffer(bufferCount),
filler()
{
    if(slot->simpleSlot->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + slot->name + " is already linked.");
    
    arg.reset(typeSpec->create());
    
    slot->simpleSlot->arg = arg.get();
    slot->simpleSlot->link = this;
    
    filler.reset(new FdFiller(addr, addrLen, linkId, &linkBuffer, typeSpec));
}

SocketLinkOutput::~SocketLinkOutput() noexcept
{
    slot->simpleSlot->link = nullptr;
    slot->simpleSlot->arg = nullptr;
}

void SocketLinkOutput::push ( SimpleSlot* src )
{
    if(src != slot->simpleSlot)
        throw InternalError("slot doesn't match with link.");
    
    auto& buffer = linkBuffer.getEmpty();
    
    if(fixedLen == 0) // Variable
    {
        buffer.clear();
        MemBufferOStream os(buffer);
        typeSpec->serialize_variable(src->arg, this->slot->localArg.get(), os);
    }
    else // Fixed
    {
        buffer.resize(fixedLen);
        typeSpec->serialize_fixed(src->arg, this->slot->localArg.get(), buffer.data());
    }
    
    linkBuffer.putFull();
}

void SocketLinkOutput::fetch ( SimpleSlot* )
{
    throw UnsupportedOperation("Fetch on output link.");
}
