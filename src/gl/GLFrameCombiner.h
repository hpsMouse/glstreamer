#ifndef _9F08DAA0_B601_11E2_A0C7_206A8A22A96A
#define _9F08DAA0_B601_11E2_A0C7_206A8A22A96A

#include "../processor.h"

namespace glstreamer_gl
{
    class GLFrameCombiner : public glstreamer::Processor
    {
    public:
        GLFrameCombiner(std::size_t nFrames);
        
    private:
        virtual void run() override final;
    };
}

#endif
