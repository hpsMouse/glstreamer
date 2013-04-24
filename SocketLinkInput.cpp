#include "SocketLinkInput.h"

#include "demuxer.h"
#include "exceptions.h"
#include "fullslot.h"
#include "linkbuffer.h"
#include "MemBufferIStream.h"
#include "slottypes.h"
#include "simpleslot.h"

using namespace glstreamer;

// Check list for link classes:
// DONE: Double link check
// DONE: Type matching
// DONE: Argument object assign and check
// DONE: Link reflection set and optionally check
// DONE: Slot pointer check
// NOTE: Type matching is done on link. No exception is thrown. To be improved.

SocketLinkInput::SocketLinkInput ( Demuxer* demuxer, Word32 linkId, const InputSlot& dest, size_type bufferCount ):
slot(&dest.toSlot()),
typeSpec(slot->typeSpec),
fixedLen(typeSpec->serialize_size()),
arg(nullptr, Destroyer(typeSpec)),
linkBuffer(nullptr)
{
    if(slot->simpleSlot->link != nullptr)
        throw std::invalid_argument("Double link detected. Slot " + slot->name + " is already linked.");
    
    arg.reset(typeSpec->create());
    
    slot->simpleSlot->arg = arg.get();
    slot->simpleSlot->link = this;
    
    linkBuffer = demuxer->getLink(linkId, bufferCount, typeSpec);
}

SocketLinkInput::~SocketLinkInput() noexcept
{
    slot->simpleSlot->link = nullptr;
    slot->simpleSlot->arg = nullptr;
}

void SocketLinkInput::push ( SimpleSlot* )
{
    throw UnsupportedOperation("Push on input link.");
}

void SocketLinkInput::fetch ( SimpleSlot* dst )
{
    if(dst != slot->simpleSlot)
        throw InternalError("slot doesn't match with link.");
    
    auto const& buffer = linkBuffer->getFull();
    if(fixedLen == 0) // Variable
    {
        MemBufferIStream is(buffer.data(), buffer.size());
        typeSpec->deserialize_varialbe(dst->arg, this->slot->localArg.get(), is);
    }
    else // Fixed
    {
        typeSpec->deserialize_fixed(dst->arg, this->slot->localArg.get(), buffer.data());
    }
    linkBuffer->putEmpty();
}
