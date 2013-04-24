#ifndef _83E3B3E8_AC84_11E2_BB76_206A8A22A96A
#define _83E3B3E8_AC84_11E2_BB76_206A8A22A96A

#include <utility>

namespace glstreamer
{
    class LocalArgBase
    {
    public:
        virtual ~LocalArgBase() {}
    };
    
    template <typename ArgType>
    class LocalArg : public LocalArgBase
    {
    public:
        template <typename ... Args>
        LocalArg(Args&& ... args):
        arg_(std::forward(args)...)
        {}
        
        virtual ~LocalArg();
        
        ArgType& arg() { return arg_; }
        
    private:
        ArgType arg_;
    };
}

#endif
