#ifndef _C4DDA6B6_2D71_11E2_B234_206A8A22A96A
#define _C4DDA6B6_2D71_11E2_B234_206A8A22A96A

namespace glstreamer
{
    class Processor;
    class Slot;
    
    class InputSlot
    {
        friend class Processor;
        explicit InputSlot(Slot& slot): slot(slot) {}
        Slot& slot;
        
    public:
        Slot& toSlot() const { return slot; }
    };
    
    class OutputSlot
    {
        friend class Processor;
        explicit OutputSlot(Slot& slot): slot(slot) {}
        Slot& slot;
        
    public:
        Slot& toSlot() const { return slot; }
    };
}

#endif
