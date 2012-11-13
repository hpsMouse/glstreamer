#ifndef _D6C228B8_2D6A_11E2_9388_206A8A22A96A
#define _D6C228B8_2D6A_11E2_9388_206A8A22A96A

#include <memory>

#include "link.h"

#include "slottypes.h"
#include "typespec.h"

namespace glstreamer
{
    class InternalSingleLink : public Link
    {
    public:
        InternalSingleLink(OutputSlot const& srcSlot, InputSlot const& dstSlot);
        virtual ~InternalSingleLink() noexcept;
        
        virtual void push(Slot*) {}
        virtual void fetch(Slot*) {}
        
    protected:
        struct Destroyer
        {
            Destroyer(TypeSpec *typeSpec): typeSpec(typeSpec) {}
            
            void operator() (void* obj){ typeSpec->destroy(obj); }
            
        private:
            TypeSpec *typeSpec;
        };
        Slot* src;
        Slot* dst;
        std::unique_ptr<void, Destroyer> arg;
    };
}

#endif
