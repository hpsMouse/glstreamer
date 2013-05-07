#ifndef _FCDA7FBA_B5FE_11E2_BA50_206A8A22A96A
#define _FCDA7FBA_B5FE_11E2_BA50_206A8A22A96A

#include "GLGenericRenderer.h"

#include "GLObject.h"

namespace glstreamer_gl
{
    class GLFBORenderer : public GLGenericRenderer
    {
    public:
        GLFBORenderer(std::size_t inputColorFrames,
                      std::size_t inputDepthFrames,
                      bool hasColorOutput = true,
                      bool hasDepthOutput = true,
                      bool show = true,
                      ProjectionStyle projection = ProjectionStyle::Frustum
                     );
        
    protected:
        virtual void setupOutputFramebuffer() override;
        virtual void drawFinish() override;
        
    private:
        FramebufferObject fbo;
        
        bool hasColor, hasDepth, show;
    };
}

#endif
