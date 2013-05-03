#ifndef _83E3B3E8_AC84_11E2_BB76_206A8A22A96A
#define _83E3B3E8_AC84_11E2_BB76_206A8A22A96A

#include <utility>

namespace glstreamer
{
    class LocalArgBase
    {
    public:
        virtual ~LocalArgBase() {}
        
        template <typename ArgType>
        ArgType& getArg();
    };
    
    template <typename ArgType>
    class LocalArg : public LocalArgBase
    {
    public:
        template <typename ... Args>
        LocalArg(Args&& ... args):
        arg_(std::forward(args)...)
        {}
        
        virtual ~LocalArg() {}
        
        ArgType& arg() { return arg_; }
        
    private:
        using LocalArgBase::getArg;
        
        ArgType arg_;
    };
    
    template <typename ArgType>
    inline ArgType& LocalArgBase::getArg()
    {
        return dynamic_cast<LocalArg<ArgType>&>(*this).arg();
    }
}

#endif
