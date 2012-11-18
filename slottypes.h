#ifndef _C4DDA6B6_2D71_11E2_B234_206A8A22A96A
#define _C4DDA6B6_2D71_11E2_B234_206A8A22A96A

#include "types.h"

namespace glstreamer
{
    class InputSlot
    {
        friend class Processor;
        explicit InputSlot(FullSlot& slot): slot(slot) {}
        FullSlot& slot;
        
    public:
        FullSlot& toSlot() const { return slot; }
    };
    
    class OutputSlot
    {
        friend class Processor;
        explicit OutputSlot(FullSlot& slot): slot(slot) {}
        FullSlot& slot;
        
    public:
        FullSlot& toSlot() const { return slot; }
    };
}

#endif
