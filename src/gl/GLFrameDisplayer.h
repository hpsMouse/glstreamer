#ifndef _27A52552_B603_11E2_9570_206A8A22A96A
#define _27A52552_B603_11E2_9570_206A8A22A96A

#include "../processor.h"

#include "GLObject.h"

#include "parameter_types.h"

namespace glstreamer_gl
{
    class GLFrameDisplayer : public glstreamer::Processor
    {
    public:
        GLFrameDisplayer();
        
    private:
        virtual void run() override;
        
        FramebufferObject fbo;
    };
}

#endif
