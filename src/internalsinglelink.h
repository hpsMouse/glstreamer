#ifndef _D6C228B8_2D6A_11E2_9388_206A8A22A96A
#define _D6C228B8_2D6A_11E2_9388_206A8A22A96A

#include <memory>

#include "link.h"

#include "types.h"
#include "typespec.h"

namespace glstreamer
{
    class InternalSingleLink : public Link
    {
    public:
        InternalSingleLink(OutputSlot const& srcSlot, InputSlot const& dstSlot);
        virtual ~InternalSingleLink() noexcept;
        
        InternalSingleLink(InternalSingleLink const&) = delete;
        InternalSingleLink& operator = (InternalSingleLink const&) = delete;
        
        virtual void push(SimpleSlot*) override {}
        virtual void fetch(SimpleSlot*) override {}
        
    protected:
        FullSlot* src;
        FullSlot* dst;
        std::unique_ptr<void, Destroyer> arg;
    };
}

#endif
