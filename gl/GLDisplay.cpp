#include "GLDisplay.h"

#include "GLXException.h"

using namespace glstreamer_gl;

GLDisplay::GLDisplay ( const char* displayName ):
display(XOpenDisplay(displayName))
{
    if(!display)
        throw GLXException("XOpenDisplay()");
}

GLDisplay::~GLDisplay() noexcept
{
    XCloseDisplay(display);
}
