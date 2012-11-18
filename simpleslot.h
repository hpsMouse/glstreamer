#ifndef _2E97DA3C_2FB5_11E2_8885_206A8A22A96A
#define _2E97DA3C_2FB5_11E2_8885_206A8A22A96A

#include "types.h"

namespace glstreamer
{
    struct SimpleSlot
    {
        explicit SimpleSlot(TypeSpec *typeSpec):
        arg(nullptr),
        link(nullptr),
        typeSpec(typeSpec)
        {}
        
        void *arg;
        Link *link;
        TypeSpec* const typeSpec;
    };
}

#endif
