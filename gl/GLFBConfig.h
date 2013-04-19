#ifndef _EC2F2840_A8BC_11E2_BA60_206A8A22A96A
#define _EC2F2840_A8BC_11E2_BA60_206A8A22A96A

#include <GL/glx.h>

#include "types.h"

namespace glstreamer_gl
{
    GLXFBConfig getBestFBConfig(GLDisplay const& display, int const attribs[]);
    
    static int const FB_ATTRIBS_RGBA[] = {
        GLX_DOUBLEBUFFER , True,
        GLX_RED_SIZE     , 8,
        GLX_GREEN_SIZE   , 8,
        GLX_BLUE_SIZE    , 8,
        GLX_ALPHA_SIZE   , 8,
        GLX_DEPTH_SIZE   , 24,
        GLX_RENDER_TYPE  , GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_X_RENDERABLE , True,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        None
    };
    
    static int const FB_ATTRIBS_EMPTY[] = {
        None
    };
}

#endif
