#include "../Plugin.h"

#include "../defaulttypespec.h"
#include "../typemgr.h"

#include "basic_types.h"
#include "string_spec.h"
#include "vector_spec.h"

namespace glstreamer_core
{
    class CorePlugin : public glstreamer::Plugin
    {
    public:
        virtual void init() override;
        virtual void registerTypes() override;
        virtual void registerProcessors() override;
    };
    
    static CorePlugin plugin_instance;
}

extern "C"
{
    ::glstreamer::Plugin* getPlugin()
    {
        return &glstreamer_core::plugin_instance;
    }
    
    char const* glstreamer_plugin_name = "glstreamer-core";
    int glstreamer_plugin_major = 1;
    int glstreamer_plugin_minor = 0;
}

void glstreamer_core::CorePlugin::init()
{}

#define GLSTREAMER_CORE_REGISTER_BASIC_TYPE(type) ::glstreamer::TypeManager::registerType< type, ::glstreamer::DefaultTypeSpec< type > >();
#define GLSTREAMER_CORE_REGISTER_VECTOR_TYPE(type) ::glstreamer::TypeManager::registerType< ::std::vector< type >, ::glstreamer_core::VectorSpec< type > >();

void glstreamer_core::CorePlugin::registerTypes()
{
    GLSTREAMER_CORE_ENUMERATE_BASIC_TYPES(GLSTREAMER_CORE_REGISTER_BASIC_TYPE)
    ::glstreamer::TypeManager::registerType<std::string, StringSpec>();
    GLSTREAMER_CORE_ENUMERATE_BASIC_TYPES(GLSTREAMER_CORE_REGISTER_VECTOR_TYPE)
    GLSTREAMER_CORE_REGISTER_VECTOR_TYPE(::std::string)
}

void glstreamer_core::CorePlugin::registerProcessors()
{}
