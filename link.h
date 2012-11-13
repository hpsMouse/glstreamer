#ifndef _6ECBB446_2D69_11E2_8978_206A8A22A96A
#define _6ECBB446_2D69_11E2_8978_206A8A22A96A

namespace glstreamer
{
    class Slot;
    
    class Link
    {
    public:
        virtual ~Link() = default;
        virtual void push(Slot *src) = 0;
        virtual void fetch(Slot *dst) = 0;
    };
}

#endif
