#include <stdexcept>

#include "internalsinglelink.h"

#include "slot.h"

using namespace glstreamer;

InternalSingleLink::InternalSingleLink(const OutputSlot& srcSlot, const InputSlot& dstSlot):
src(&srcSlot.toSlot()),
dst(&dstSlot.toSlot()),
arg(nullptr, Destroyer(src->typeSpec))
{
    if(src->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + src->name + " is already linked.");
    if(dst->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + dst->name + " is already linked.");
    if(src->typeSpec != dst->typeSpec)
        throw std::invalid_argument(std::string("Types don't match: ") + src->typeSpec->id()->name() + " against " + dst->typeSpec->id()->name());
    
    arg.reset(src->typeSpec->create());
    
    dst->arg = src->arg = arg.get();
    dst->link = src->link = this;
}

InternalSingleLink::~InternalSingleLink() noexcept
{
    dst->arg = src->arg = nullptr;
    dst->link = src->link = nullptr;
}
