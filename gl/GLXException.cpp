#include <string>
#include <vector>

#include "GLXException.h"

static std::string getErrorText(Display *display, int code, int length) noexcept
{
    std::vector<char> buf(length);
    if(!XGetErrorText(display, code, buf.data(), length))
        return "Unknown error";
    return std::string(buf.data());
}

glstreamer_gl::GLXException::GLXException ( Display* display, int code ) :
runtime_error("GLX error: " + getErrorText(display, code, 1024)),
display(display)
{}

glstreamer_gl::GLXException::GLXException ( const std::string& __arg, Display* display ) :
runtime_error ( "GLX error: " + __arg ),
display(display)
{}

int glstreamer_gl::GLXException::XlibErrorHandler ( Display* display, XErrorEvent* event )
{
    throw GLXException(display, event->error_code);
}

int glstreamer_gl::GLXException::XlibIOErrorHandler ( Display* display )
{
    throw GLXException("Fatal I/O error.", display);
}
