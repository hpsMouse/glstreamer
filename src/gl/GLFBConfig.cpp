#include "GLFBConfig.h"

#include "GLDisplay.h"

#include "GLXException.h"

using namespace glstreamer_gl;

GLXFBConfig glstreamer_gl::getBestFBConfig ( const GLDisplay& display, const int attribs[] )
{
    int nConfigs = 0;
    GLXFBConfig *configs = glXChooseFBConfig(display.toDisplay(), DefaultScreen(display.toDisplay()), attribs, &nConfigs);
    if(!configs)
        throw GLXException("glXChooseFBConfig()");
    
    GLXFBConfig result = configs[0];
    XFree(configs);
    
    return result;
}
