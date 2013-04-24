#include <string>

#include <X11/Xlib.h>

#include "GLContext.h"

#include "GLDisplay.h"

#include "GLXException.h"

using namespace glstreamer_gl;

GLContext::GLContext ( const GLDisplay* display, GLXFBConfig config ):
display(display),
context(glXCreateNewContext(display->toDisplay(), config, GLX_RGBA_TYPE, nullptr, True))
{
    if(!context)
        throw GLXException("glXCreateNewContext()");
}

GLContext::~GLContext() noexcept
{
    glXDestroyContext(display->toDisplay(), context);
}

void GLContext::makeCurrent ( GLXDrawable drawable )
{
    if(!glXMakeCurrent(display->toDisplay(), drawable, context))
        throw GLXException("glXMakeCurrent()");
}

void GLContext::releaseCurrent() noexcept
{
    glXMakeCurrent(display->toDisplay(), None, nullptr);
}

void GLContext::printInfo()
{
    printf("Vendor: %s\n", (char const*)glGetString(GL_VENDOR));
    printf("Renderer: %s\n", (char const*)glGetString(GL_RENDERER));
    printf("Version: %s\n", (char const*)glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
