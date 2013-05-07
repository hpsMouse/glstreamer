#ifndef _4F48D4BA_B64D_11E2_BC45_206A8A22A96A
#define _4F48D4BA_B64D_11E2_BC45_206A8A22A96A

#include "../processor.h"

namespace glstreamer_core
{
    template <typename T>
    class VariableProcessor : public glstreamer::Processor
    {
    public:
        VariableProcessor():
        glstreamer::Processor(),
        value()
        {
            outputArgs.addSlot<T>("output");
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
        
        virtual void run() override
        {
            *static_cast<T*>(outputArgs.simpleSlot(0).arg) = value;
        }
    };
}

#endif
