#ifndef _1946BB36_A8B8_11E2_8551_206A8A22A96A
#define _1946BB36_A8B8_11E2_8551_206A8A22A96A

#include <GL/glx.h>

#include <X11/Xlib.h>

#include "types.h"

namespace glstreamer_gl
{
    class GLWindow
    {
    public:
        GLWindow(GLDisplay const* display, GLXFBConfig config, unsigned width, unsigned height);
        ~GLWindow() noexcept;
        
        void map();
        void unmap();
        
        Window toWindow() const
        {
            return window;
        }
        
    private:
        GLWindow(GLWindow const&) = delete;
        GLWindow& operator = (GLWindow const&) = delete;
        
        GLDisplay const* display;
        Window window;
    };
}

#endif
