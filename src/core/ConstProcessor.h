#ifndef _160EB68A_B547_11E2_9BF9_206A8A22A96A
#define _160EB68A_B547_11E2_9BF9_206A8A22A96A

#include <type_traits>

#include "../processor.h"

namespace glstreamer_core
{
    template <typename Type>
    class ConstProcessor : public glstreamer::Processor
    {
    public:
        ConstProcessor(Type const& value):
        value(value)
        {
            outputArgs.addSlot<Type>("output");
        }
        
    private:
        virtual void run()
        {
            *static_cast<Type*>(outputArgs.simpleSlot(0).arg) = value;
        }
        
        typename std::decay<Type>::type value;
    };
}

#endif
