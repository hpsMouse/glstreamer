#ifndef _77B6C41E_B64A_11E2_B051_206A8A22A96A
#define _77B6C41E_B64A_11E2_B051_206A8A22A96A

#include <cstdio>

#include "../processor.h"

namespace glstreamer_core
{
    template <typename T>
    class Dispatcher : public glstreamer::Processor
    {
    public:
        Dispatcher(std::size_t numDuplications):
        glstreamer::Processor(),
        numDuplications(numDuplications)
        {
            inputArgs.addSlot<T>("input");
            for(std::size_t i = 0; i < numDuplications; ++i)
            {
                char name[64];
                std::snprintf(name, 64, "output_%zu", i);
                outputArgs._addSlot<T>(name);
            }
            outputArgs.refreshSimpleSlots();
        }
        
    private:
        std::size_t numDuplications;
        
        virtual void run() override final
        {
            auto const& input = *static_cast<T*>(inputArgs.simpleSlot(0).arg);
            for(std::size_t i = 0; i < numDuplications; ++i)
            {
                auto& output = *static_cast<T*>(outputArgs.simpleSlot(i).arg);
                output = input;
            }
        }
    };
}

#endif
