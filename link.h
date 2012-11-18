#ifndef _6ECBB446_2D69_11E2_8978_206A8A22A96A
#define _6ECBB446_2D69_11E2_8978_206A8A22A96A

#include "types.h"

namespace glstreamer
{
    class Link
    {
    public:
        virtual ~Link() = default;
        virtual void push(SimpleSlot *src) = 0;
        virtual void fetch(SimpleSlot *dst) = 0;
    };
}

#endif
