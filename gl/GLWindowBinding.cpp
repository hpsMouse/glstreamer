#include "GLWindowBinding.h"

#include "GLFBConfig.h"

using namespace glstreamer_gl;

GLWindowBinding::GLWindowBinding ( const char* displayName, unsigned int width, unsigned int height, bool show ):
display(displayName),
config(getBestFBConfig(display, FB_ATTRIBS_RGBA)),
context(&display, config),
window(&display, config, width, height)
{
    context.makeCurrent(window.toWindow());
    
    if(show)
        window.map();
}

GLWindowBinding::~GLWindowBinding() noexcept
{
    window.unmap();
    context.releaseCurrent();
}

void GLWindowBinding::swapBuffers()
{
    glXSwapBuffers(display.toDisplay(), window.toWindow());
}
