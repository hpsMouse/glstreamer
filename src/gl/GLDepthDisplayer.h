#ifndef _1DBAB7F2_B85E_11E2_B5C9_206A8A22A96A
#define _1DBAB7F2_B85E_11E2_B5C9_206A8A22A96A

#include "../processor.h"

#include "GLProgram.h"
#include "GLShader.h"

namespace glstreamer_gl
{
    class GLDepthDisplayer : public glstreamer::Processor
    {
    public:
        GLDepthDisplayer();
        
    private:
        GLProgram program;
        GLShader shader;
        
        virtual void run() override;
    };
}

#endif
