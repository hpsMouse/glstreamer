#include "threadedlink.h"

#include "exceptions.h"
#include "fullslot.h"
#include "simpleslot.h"
#include "slottypes.h"

using namespace std;
using namespace glstreamer;

ThreadedLink::ThreadedLink(const OutputSlot& srcSlot, const InputSlot& dstSlot, size_type queueSize):
src(&srcSlot.toSlot()),
dst(&dstSlot.toSlot()),
queueSize(queueSize),
typeSpec(srcSlot.toSlot().typeSpec),
argBuffers(new std::unique_ptr<void, Destroyer>[queueSize]),
currentSize(2),
indexIn(0),
indexOut(queueSize - 1),
queueLock(),
condPush(),
condFetch()
{
    if(src->simpleSlot->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + src->name + " is already linked.");
    if(dst->simpleSlot->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + dst->name + " is already linked.");
    if(src->typeSpec != dst->typeSpec)
        throw std::invalid_argument(std::string("Types don't match: ") + src->typeSpec->id().name() + " against " + dst->typeSpec->id().name());
    
    for(size_type i = 0; i < queueSize; ++i)
        argBuffers[i] = std::unique_ptr<void, Destroyer>(typeSpec->create(), Destroyer(typeSpec));
    
    src->simpleSlot->arg = argBuffers[indexIn].get();
    dst->simpleSlot->arg = argBuffers[indexOut].get();
    dst->simpleSlot->link = src->simpleSlot->link = this;
}

ThreadedLink::~ThreadedLink() noexcept
{
    dst->simpleSlot->arg = src->simpleSlot->arg = nullptr;
    dst->simpleSlot->link = src->simpleSlot->link = nullptr;
}

void ThreadedLink::push(SimpleSlot* src)
{
    if(src != this->src->simpleSlot)
        throw InternalError("slot doesn't match with link.");
    
    std::unique_lock<mutex> locker(queueLock);
    while(currentSize >= queueSize)
        condPush.wait(locker);
    
    if(src->arg != argBuffers[indexIn].get())
        throw InternalError("argument reference differs between slot and link");
    
    typeSpec->context_out(src->arg, this->src->localArg.get());
    
    indexIn = (indexIn + 1) % queueSize;
    ++currentSize;
    src->arg = argBuffers[indexIn].get();
    
    if(currentSize == 3)
        condFetch.notify_one();
}

void ThreadedLink::fetch(SimpleSlot* dst)
{
    if(dst != this->dst->simpleSlot)
        throw InternalError("slot doesn't match with link.");
    
    std::unique_lock<mutex> locker(queueLock);
    while(currentSize == 2)
        condFetch.wait(locker);
    
    if(dst->arg != argBuffers[indexOut].get())
        throw InternalError("argument reference differs between slot and link");
    
    indexOut = (indexOut + 1) % queueSize;
    --currentSize;
    dst->arg = argBuffers[indexOut].get();
    
    typeSpec->context_in(dst->arg, this->dst->localArg.get());
    
    if(currentSize == queueSize - 1)
        condPush.notify_one();
}
