#ifndef _BF21C8F2_A8BD_11E2_8AA6_206A8A22A96A
#define _BF21C8F2_A8BD_11E2_8AA6_206A8A22A96A

#include "GLContext.h"
#include "GLDisplay.h"

namespace glstreamer_gl
{
    class GLContextBinding
    {
    public:
        GLContextBinding(char const* displayName);
        ~GLContextBinding() noexcept;
        
        GLDisplay &getDisplay()
        {
            return display;
        }
        
        GLContext &getContext()
        {
            return context;
        }
        
    private:
        GLContextBinding(GLContextBinding const&) = delete;
        GLContextBinding& operator = (GLContextBinding const&) = delete;
        
        GLDisplay display;
        GLContext context;
    };
}

#endif
