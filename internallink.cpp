#include "internallink.h"

#include "processor.h"
#include "typespec.h"

using namespace glstreamer;

void InternalLink::Destroyer::operator()(void* obj)
{
    typeSpec->destroy(obj);
}

InternalLink::InternalLink(Processor* src, std::size_t srcSlot, Processor* dst, std::size_t dstSlot):
src(src),
dst(dst),
srcSlot(&src->outputArg(srcSlot)),
dstSlot(&dst->inputArg(dstSlot)),
typeSpec(this->srcSlot->typeSpec),
arg(nullptr, Destroyer(typeSpec))
{
    if(this->srcSlot->arg != nullptr)
        throw std::invalid_argument(this->srcSlot->name + " is already linked.");
    
    if(this->dstSlot->arg != nullptr)
        throw std::invalid_argument(this->dstSlot->name + " is already linked.");
    
    if(*this->srcSlot->typeSpec->id() != *this->dstSlot->typeSpec->id())
        throw std::invalid_argument("Uncompatible types between " + this->srcSlot->name + " and " + this->dstSlot->name + ".");
    
    arg.reset(typeSpec->create());
    
    this->srcSlot->arg = arg.get();
    this->dstSlot->arg = arg.get();
}

InternalLink::~InternalLink()
{
    this->srcSlot->arg = nullptr;
    this->dstSlot->arg = nullptr;
}
