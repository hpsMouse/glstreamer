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
        explicit ConstProcessor(Type const& value, std::size_t count = 1):
        value(value),
        count(count)
        {
            for(std::size_t i = 0; i < count; ++i)
                outputArgs._addSlot<Type>(genName("output", i));
            outputArgs.refreshSimpleSlots();
        }
        
    private:
        virtual void run()
        {
            for(std::size_t i = 0; i < count; ++i)
                *static_cast<Type*>(outputArgs.simpleSlot(i).arg) = value;
        }
        
        typename std::decay<Type>::type value;
        std::size_t count;
    };
}

#endif
