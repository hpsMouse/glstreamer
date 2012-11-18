#include <stdexcept>

#include "internalsinglelink.h"

#include "fullslot.h"
#include "simpleslot.h"
#include "slottypes.h"

using namespace glstreamer;

InternalSingleLink::InternalSingleLink(const OutputSlot& srcSlot, const InputSlot& dstSlot):
src(&srcSlot.toSlot()),
dst(&dstSlot.toSlot()),
arg(nullptr, Destroyer(src->typeSpec))
{
    if(src->simpleSlot->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + src->name + " is already linked.");
    if(dst->simpleSlot->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + dst->name + " is already linked.");
    if(src->typeSpec != dst->typeSpec)
        throw std::invalid_argument(std::string("Types don't match: ") + src->typeSpec->id().name() + " against " + dst->typeSpec->id().name());
    
    arg.reset(src->typeSpec->create());
    
    dst->simpleSlot->arg = src->simpleSlot->arg = arg.get();
    dst->simpleSlot->link = src->simpleSlot->link = this;
}

InternalSingleLink::~InternalSingleLink() noexcept
{
    dst->simpleSlot->arg = src->simpleSlot->arg = nullptr;
    dst->simpleSlot->link = src->simpleSlot->link = nullptr;
}
