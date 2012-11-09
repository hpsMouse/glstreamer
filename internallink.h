#ifndef _19DD4E7A_2A64_11E2_AC06_206A8A22A96A
#define _19DD4E7A_2A64_11E2_AC06_206A8A22A96A

#include <cstddef>

#include <memory>
#include <string>

namespace glstreamer
{
    class Processor;
    class Slot;
    class TypeSpec;
    class InternalLink
    {
    public:
        InternalLink(Processor *src, std::size_t srcSlot, Processor *dst, std::size_t dstSlot);
        ~InternalLink();
        
    private:
        struct Destroyer
        {
            Destroyer(TypeSpec const* typeSpec):
            typeSpec(typeSpec)
            {}
            
            void operator() (void *obj);
            
        private:
            TypeSpec const* typeSpec;
        };
        
        Processor *src, *dst;
        Slot *srcSlot, *dstSlot;
        TypeSpec const* typeSpec;
        
        std::unique_ptr<void, Destroyer> arg;
    };
}

#endif
