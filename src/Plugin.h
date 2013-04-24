#ifndef _86ED81F8_960F_11E2_842E_206A8A22A96A
#define _86ED81F8_960F_11E2_842E_206A8A22A96A

namespace glstreamer
{
    class Plugin
    {
    public:
        virtual ~Plugin() = default;
        virtual void init()               = 0;
        virtual void registerTypes()      = 0;
        virtual void registerProcessors() = 0;
    };
    
    // Every plugin should export a function of the following type:
    extern "C" typedef ::glstreamer::Plugin* getPlugin();
}

#endif
