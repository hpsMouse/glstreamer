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
        struct ColorOnly {};
        
        GLFrameComposer(std::size_t nFrames);
        GLFrameComposer(std::size_t nFrames, ColorOnly const&);
        GLFrameComposer(std::size_t nFrames, std::vector<GLFrameInfo>&& frameInfo, std::size_t colorFrames, std::size_t depthFrames);
        
        static std::vector<GLFrameInfo> makeTrivialFrameList(std::size_t nFrames);
        static std::vector<GLFrameInfo> makeColorOnlyFrameList(std::size_t nFrames);
        
    private:
        std::size_t nFrames;
        std::vector<GLFrameInfo> frameInfo;
        std::size_t colorFrames, depthFrames;
        
        FramebufferObject fbo;
        GLProgram program;
        GLShader shader;
        
        GLViewport& canvasViewportIn();
        GLViewport& canvasViewportOut();
        
        GLViewport& frameViewport(std::ptrdiff_t index);
        
        GLTextureData<RGBAFrame>& inputColorTexture(std::ptrdiff_t index);
        GLTextureData<DepthFrame>& inputDepthTexture(std::ptrdiff_t index);
        
        GLTextureData<RGBAFrame>& outputColorTexture();
        GLTextureData<DepthFrame>& outputDepthTexture();
        
        virtual void run() override;
    };
}

#endif
