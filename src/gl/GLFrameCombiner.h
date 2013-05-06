#ifndef _9F08DAA0_B601_11E2_A0C7_206A8A22A96A
#define _9F08DAA0_B601_11E2_A0C7_206A8A22A96A

#include "../processor.h"

#include "GLObject.h"

#include "GLProgram.h"
#include "GLShader.h"

namespace glstreamer_gl
{
    /**
     * Input: [ColorFrames], [DepthFrames]
     * Output: ColorFrame, DepthFrame
     */
    class GLFrameCombiner : public glstreamer::Processor
    {
    public:
        GLFrameCombiner(std::size_t nFrames, unsigned width, unsigned height);
        
        glstreamer::InputSlot colorInput(unsigned index)
        {
            return inputArg(index);
        }
        
        glstreamer::InputSlot depthInput(unsigned index)
        {
            return inputArg(nFrames + index);
        }
        
        glstreamer::OutputSlot colorOutput()
        {
            return outputArg(0);
        }
        
        glstreamer::OutputSlot depthOutput()
        {
            return outputArg(1);
        }
        
    private:
        virtual void run() override final;
        
        std::size_t nFrames;
        unsigned width, height;
        
        FramebufferObject fbo;
        GLProgram program;
        GLShader shader;
    };
}

#endif
