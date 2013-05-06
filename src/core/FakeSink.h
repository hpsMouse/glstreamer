#ifndef _867CE1AA_B603_11E2_BE16_206A8A22A96A
#define _867CE1AA_B603_11E2_BE16_206A8A22A96A

#include "../processor.h"

namespace glstreamer_core
{
    template <typename T>
    class FakeSink : public glstreamer::Processor
    {
    public:
        FakeSink() : glstreamer::Processor()
        {
            inputArgs.addSlot<T>("sink");
        }
        
    private:
        virtual void run() {}
    };
}

#endif
