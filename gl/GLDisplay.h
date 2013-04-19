#ifndef _1E6D32BC_A8B7_11E2_B41C_206A8A22A96A
#define _1E6D32BC_A8B7_11E2_B41C_206A8A22A96A

#include <X11/Xlib.h>

namespace glstreamer_gl
{
    class GLDisplay
    {
    public:
        GLDisplay(char const* displayName);
        ~GLDisplay() noexcept;
        
        Display* toDisplay() const
        {
            return display;
        }
        
    private:
        GLDisplay(GLDisplay const&) = delete;
        GLDisplay& operator = (GLDisplay const&) = delete;
        
        Display *display;
    };
}

#endif
