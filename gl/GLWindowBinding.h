#ifndef _8BD8FED2_A8BF_11E2_8E53_206A8A22A96A
#define _8BD8FED2_A8BF_11E2_8E53_206A8A22A96A

#include "GLContext.h"
#include "GLDisplay.h"
#include "GLWindow.h"

namespace glstreamer_gl
{
    class GLWindowBinding
    {
    public:
        GLWindowBinding(char const* displayName, unsigned width, unsigned height, bool show = true, int x = 0, int y = 0);
        ~GLWindowBinding() noexcept;
        
        void swapBuffers();
        
        GLDisplay& getDisplay()
        {
            return display;
        }
        
        GLContext& getContext()
        {
            return context;
        }
        
        GLWindow& getWindow()
        {
            return window;
        }
        
    private:
        GLWindowBinding(GLWindowBinding const&) = delete;
        GLWindowBinding& operator = (GLWindowBinding const&) = delete;
        
        GLDisplay display;
        
        GLXFBConfig config;
        
        GLContext context;
        GLWindow window;
    };
}

#endif
