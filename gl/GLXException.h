#ifndef _501015E8_A7FC_11E2_B9C4_206A8A22A96A
#define _501015E8_A7FC_11E2_B9C4_206A8A22A96A

#include <stdexcept>

#include <X11/Xlib.h>

namespace glstreamer_gl
{
    class GLXException : public std::runtime_error
    {
    public:
        GLXException(Display *display, int code);
        explicit GLXException ( const std::string& __arg, Display *display = nullptr );
        
        GLXException(GLXException const&) = default;
        GLXException& operator = (GLXException const&) = default;
        
        static int XlibErrorHandler(Display * display, XErrorEvent * event) __attribute__((noreturn));
        static int XlibIOErrorHandler(Display * display) __attribute__((noreturn));
        
    private:
        Display *display;
    };
}

#endif
