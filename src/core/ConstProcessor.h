#ifndef _160EB68A_B547_11E2_9BF9_206A8A22A96A
#define _160EB68A_B547_11E2_9BF9_206A8A22A96A

#include <cstdio>

#include <type_traits>

#include "../processor.h"

namespace glstreamer_core
{
    template <typename Type>
    class ConstProcessor : public glstreamer::Processor
    {
    public:
        ConstProcessor(Type const& value, std::size_t count = 1):
        value(value),
        count(count)
        {
            for(std::size_t i = 0; i < count; ++i)
            {
                char name[64];
                std::sprintf(name, "output_%zu", i);
                outputArgs._addSlot<Type>(name);
            }
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
