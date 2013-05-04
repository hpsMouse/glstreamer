#include "../Plugin.h"
#include "../typemgr.h"

#include "GLFrameTypeSpec.h"

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

template <typename FrameType>
static inline void registerFrameType()
{
    glstreamer::TypeManager::registerType<glstreamer_gl::GLFrameData<FrameType>, glstreamer_gl::GLFrameTypeSpec<FrameType>>();
}

void glstreamer_gl::GLPlugin::registerTypes()
{
    registerFrameType<RGBAFrame>();
    registerFrameType<DepthFrame>();
}

void glstreamer_gl::GLPlugin::registerProcessors()
{

}
