#ifndef _D80EEDFC_B53A_11E2_9354_206A8A22A96A
#define _D80EEDFC_B53A_11E2_9354_206A8A22A96A

#include <cstddef>

#include "../processor.h"

#include "GLFrame.h"

#include "parameter_types.h"

namespace glstreamer_gl
{
    class GLGenericRenderer : public glstreamer::Processor
    {
    public:
        GLGenericRenderer(std::size_t inputColorFrames,
                          std::size_t inputDepthFrames,
                          std::size_t outputColorFrames,
                          std::size_t outputDepthFrames,
                          ProjectionStyle projection = ProjectionStyle::Frustum
                         );
        
    protected:
        virtual void setupOutputFramebuffer();
        virtual void drawFinish();
        
        virtual void draw() = 0;
        
        GLTextureData<RGBAFrame>& getInputColorFrame(std::ptrdiff_t index);
        GLTextureData<DepthFrame>& getInputDepthFrame(std::ptrdiff_t index);
        GLTextureData<RGBAFrame>& getOutputColorFrame(std::ptrdiff_t index);
        GLTextureData<DepthFrame>& getOutputDepthFrame(std::ptrdiff_t index);
        
        int getInputColorTexture(std::ptrdiff_t index);
        int getInputDepthTexture(std::ptrdiff_t index);
        
        GLViewport const& getViewport()
        {
            return *static_cast<GLViewport*>(inputArgs.simpleSlot(0).arg);
        }
        
        GLDataRange const& getDataRange()
        {
            return *static_cast<GLDataRange*>(inputArgs.simpleSlot(1).arg);
        }
        
    private:
        virtual void run() override final;
        
        std::size_t inputColorFrames, inputDepthFrames, outputColorFrames, outputDepthFrames;
        
        ProjectionStyle projection;
        
        static constexpr std::size_t fixedInputs = 2, fixedOutputs = 0;
    };
}

#endif
