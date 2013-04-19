#include <X11/Xlib.h>

#include "../Plugin.h"

#include "GLXException.h"

namespace glstreamer_gl
{
    class GLPlugin : public glstreamer::Plugin
    {
    public:
        virtual void init() override;
        virtual void registerTypes() override;
        virtual void registerProcessors() override;
    };
    
    static GLPlugin plugin_instance;
}

extern "C"
{
    ::glstreamer::Plugin* getPlugin()
    {
        return &glstreamer_gl::plugin_instance;
    }
    
    char const* glstreamer_plugin_name = "glstreamer-gl";
    int glstreamer_plugin_major = 1;
    int glstreamer_plugin_minor = 0;
}

void glstreamer_gl::GLPlugin::init()
{
    if(!XInitThreads())
        throw GLXException("XInitThreads() failed.");
}

void glstreamer_gl::GLPlugin::registerTypes()
{

}

void glstreamer_gl::GLPlugin::registerProcessors()
{

}
