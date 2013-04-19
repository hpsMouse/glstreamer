#include "GLWindow.h"

#include "GLDisplay.h"
#include "GLXException.h"

using namespace glstreamer_gl;

GLWindow::GLWindow ( const GLDisplay* display, GLXFBConfig config, unsigned int width, unsigned int height ):
display(display),
window(0)
{
    XVisualInfo *visual_info = glXGetVisualFromFBConfig(display->toDisplay(), config);
    if(!visual_info)
        throw GLXException("glXGetVisualFromFBConfig()");
    
    XSetWindowAttributes swa;
    
    window = XCreateWindow(display->toDisplay(), DefaultRootWindow(display->toDisplay()), 0, 0, width, height, 0, visual_info->depth, InputOutput, visual_info->visual, 0, &swa);
    
    XFree(visual_info);
    
    if(!window)
        throw GLXException("XCreateWindow()");
    
    XStoreName(display->toDisplay(), window, "OpenGL window");
}

GLWindow::~GLWindow() noexcept
{
    XUnmapWindow(display->toDisplay(), window);
    XDestroyWindow(display->toDisplay(), window);
}

void GLWindow::map()
{
    XMapWindow(display->toDisplay(), window);
}

void GLWindow::unmap()
{
    XUnmapWindow(display->toDisplay(), window);
}
