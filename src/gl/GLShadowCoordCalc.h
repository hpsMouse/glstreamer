#ifndef _67745518_B8AB_11E2_B963_206A8A22A96A
#define _67745518_B8AB_11E2_B963_206A8A22A96A

#include "../fixedprocessor.h"

#include "parameter_types.h"

namespace glstreamer_gl
{
    /**
     * Input: Light's Modelview and Perspective-Viewport parameter
     * Output: ModelviewPerspective matrix
     */
    class GLShadowCoordCalc : public glstreamer::FixedProcessor<glstreamer::TypeList<GLObjectState, GLViewport>, glstreamer::TypeList<GLMatrix>>
    {
    public:
        GLShadowCoordCalc(ProjectionStyle projection);
        
    private:
        ProjectionStyle projection;
        
        virtual void do_run(GLObjectState& state, GLViewport& viewport, GLMatrix& matrix) override;
    };
}

#endif
