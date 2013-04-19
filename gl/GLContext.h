#ifndef _4258D892_A7FA_11E2_B286_206A8A22A96A
#define _4258D892_A7FA_11E2_B286_206A8A22A96A

#include <GL/glx.h>

#include "types.h"

namespace glstreamer_gl
{
    class GLContext
    {
    public:
        GLContext(GLDisplay const* display, GLXFBConfig config);
        ~GLContext() noexcept;
        
        void makeCurrent(GLXDrawable drawable);
        void releaseCurrent() noexcept;
        
        void printInfo();
        
    private:
        GLContext(GLContext const&) = delete;
        GLContext& operator = (GLContext const&) = delete;
        
        GLDisplay const* display;
        GLXContext context;
    };
}

#endif
