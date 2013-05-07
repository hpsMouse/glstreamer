#ifndef _F9D54F68_B4B7_11E2_8D77_206A8A22A96A
#define _F9D54F68_B4B7_11E2_8D77_206A8A22A96A

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
    
    struct GLFrameInfo
    {
        int colorIndex, depthIndex;
    };
}

#endif
