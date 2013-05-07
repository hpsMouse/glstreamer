#ifndef _867CE1AA_B603_11E2_BE16_206A8A22A96A
#define _867CE1AA_B603_11E2_BE16_206A8A22A96A

#include "../processor.h"

namespace glstreamer_core
{
    template <typename T>
    class FakeSink : public glstreamer::Processor
    {
    public:
        explicit FakeSink(std::size_t count = 1) : glstreamer::Processor()
        {
            for(std::size_t i = 0; i < count; ++i)
                inputArgs.addSlot<T>(genName("sink", i));
        }
        
    private:
        virtual void run() {}
    };
}

#endif
