#ifndef _4F48D4BA_B64D_11E2_BC45_206A8A22A96A
#define _4F48D4BA_B64D_11E2_BC45_206A8A22A96A

#include "../processor.h"

namespace glstreamer_core
{
    template <typename T>
    class VariableProcessor : public glstreamer::Processor
    {
    public:
        explicit VariableProcessor(std::size_t numOutputs = 1):
        glstreamer::Processor(),
        value(),
        numOutputs(numOutputs)
        {
            for(std::size_t i = 0; i < numOutputs; ++i)
                outputArgs._addSlot<T>(genName("output", i));
            outputArgs.refreshSimpleSlots();
        }
        
        T& get()
        {
            return value;
        }
        
        void set(T const& newValue)
        {
            value = newValue;
        }
        
    private:
        T value;
        
        std::size_t numOutputs;
        
        virtual void run() override
        {
            for(std::size_t i = 0; i < numOutputs; ++i)
                *static_cast<T*>(outputArgs.simpleSlot(i).arg) = value;
        }
    };
}

#endif
