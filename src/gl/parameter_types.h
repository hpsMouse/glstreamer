#ifndef _F9D54F68_B4B7_11E2_8D77_206A8A22A96A
#define _F9D54F68_B4B7_11E2_8D77_206A8A22A96A

#include "gl.inc.h"

namespace glstreamer_gl
{
    struct GLViewport
    {
        int x, y;
        unsigned width, height;
        double left, right, bottom, top, near, far;
    };
    
    struct ScreenPart
    {
        double left, right, bottom, top;
    };
    
    struct GLDataRange
    {
        double start, end;
    };
    
    enum class ProjectionStyle
    {
        Frustum,
        Ortho
    };
    
    inline void applyViewport(GLViewport const& viewport, ProjectionStyle projection)
    {
        switch(projection)
        {
            case ProjectionStyle::Frustum:
                gl_Call(glFrustum(viewport.left, viewport.right, viewport.bottom, viewport.top, viewport.near, viewport.far));
                break;
            case ProjectionStyle::Ortho:
                gl_Call(glOrtho(viewport.left, viewport.right, viewport.bottom, viewport.top, viewport.near, viewport.far));
                break;
        }
    }
    
    struct GLFrameInfo
    {
        int colorIndex, depthIndex;
    };
    
    struct GLObjectState
    {
        double scale;
        double posx, posy, posz;
        double rx, ry, rz;
        
        void apply() const
        {
            gl_Call(glTranslated(posx, posy, posz));
            gl_Call(glRotated(rz, 0., 0., 1.));
            gl_Call(glRotated(ry, 0., 1., 0.));
            gl_Call(glRotated(rx, 1., 0., 0.));
            gl_Call(glScaled(scale, scale, scale));
        }
    };
    
    struct GLMatrix
    {
        float elements[16];
    };
}

#endif
