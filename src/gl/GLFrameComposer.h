#ifndef _68AD1E3E_B6ED_11E2_B394_206A8A22A96A
#define _68AD1E3E_B6ED_11E2_B394_206A8A22A96A

#include "../processor.h"

#include "GLFrame.h"
#include "GLProgram.h"
#include "GLShader.h"

#include "parameter_types.h"

namespace glstreamer_gl
{
    /**
     * Input: Viewport, [Viewport], [ColorFrame], [DepthFrame]
     * Output: Viewport, ColorFrame, DepthFrame
     */
    class GLFrameComposer : public glstreamer::Processor
    {
    public:
        GLFrameComposer(std::size_t nFrames, std::vector<GLFrameInfo>&& frameInfo, std::size_t colorFrames, std::size_t depthFrames);
        
        glstreamer::InputSlot viewportInputSlot()
        {
            return inputArg(0);
        }
        
        glstreamer::InputSlot viewportInputSlot(std::ptrdiff_t index)
        {
            return inputArg(1 + index);
        }
        
        glstreamer::InputSlot colorInputSlot(std::ptrdiff_t index)
        {
            return inputArg(1 + nFrames + index);
        }
        
        glstreamer::InputSlot depthInputSlot(std::ptrdiff_t index)
        {
            return inputArg(1 + nFrames + colorFrames + index);
        }
        
        glstreamer::OutputSlot viewportOutputSlot()
        {
            return outputArg(0);
        }
        
        glstreamer::OutputSlot colorOutputSlot()
        {
            return outputArg(1);
        }
        
        glstreamer::OutputSlot depthOutputSlot()
        {
            return outputArg(2);
        }
        
    private:
        std::size_t nFrames;
        std::vector<GLFrameInfo> frameInfo;
        std::size_t colorFrames, depthFrames;
        
        FramebufferObject fbo;
        GLProgram program;
        GLShader shader;
        
        GLViewport& getViewport();
        
        GLViewport& viewportInput(std::ptrdiff_t index);
        
        GLTextureData<RGBAFrame>& inputColorTexture(std::ptrdiff_t index);
        GLTextureData<DepthFrame>& inputDepthTexture(std::ptrdiff_t index);
        
        GLViewport& getOutputViewport();
        
        GLTextureData<RGBAFrame>& outputColorTexture();
        GLTextureData<DepthFrame>& outputDepthTexture();
        
        virtual void run() override;
    };
}

#endif
