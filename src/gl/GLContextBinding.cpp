#include "GLContextBinding.h"

#include "GLFBConfig.h"

using namespace glstreamer_gl;

GLContextBinding::GLContextBinding ( const char* displayName ):
display(displayName),
context(&display, getBestFBConfig(display, FB_ATTRIBS_EMPTY))
{
    context.makeCurrent(DefaultRootWindow(display.toDisplay()));
}

GLContextBinding::~GLContextBinding() noexcept
{
    context.releaseCurrent();
}
