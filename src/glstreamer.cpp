#include <cstdio>

#include <iostream>

#include "glstreamer.h"

#include "Plugin.h"

#include <posixpp/BitField.h>
#include <posixpp/PosixExecutableObject.h>

#include <posixpp/constants/constants.h>

void glstreamer::init()
{
    loadExternalPlugin("./libglstreamer_core.so");
    loadExternalPlugin("./libglstreamer_gl.so");
}

void glstreamer::loadExternalPlugin ( const char* filename )
{
    using namespace posixpp;
    PosixExecutableObject dlobj(filename, LinkPolicy::Now, LinkVisibility::Local);
    
    char const* plugin_name = dlobj.object<char const*>("glstreamer_plugin_name");
    int plugin_major = dlobj.object<int>("glstreamer_plugin_major");
    int plugin_minor = dlobj.object<int>("glstreamer_plugin_minor");
    
    std::clog << "Loading plugin " << plugin_name << ' ' << plugin_major << '.' << plugin_minor << std::endl;
    
    Plugin *plugin = dlobj.function<Plugin*()>("getPlugin")();
    plugin->init();
    plugin->registerTypes();
    plugin->registerProcessors();
}
