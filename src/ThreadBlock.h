#ifndef _BB88AF00_B874_11E2_A0AC_206A8A22A96A
#define _BB88AF00_B874_11E2_A0AC_206A8A22A96A

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "types.h"

namespace glstreamer
{
    class ThreadBlock
    {
    public:
        ThreadBlock();
        ~ThreadBlock();
        
        ThreadBlock& addProcessor(std::string const& name, Processor* processor);
        
        template <typename ProcessorType, typename ... Args>
        ThreadBlock& addProcessor(std::string const& name, Args&& ... args)
        {
            return addProcessor(name, new ProcessorType(std::forward<Args>(args)...));
        }
        
        void execute();
        
        Processor& processor(std::string const& name);
        
        Processor& operator[] (std::string const& name)
        {
            return processor(name);
        }
        
    private:
        ThreadBlock(ThreadBlock const&) = delete;
        ThreadBlock& operator = (ThreadBlock const&) = delete;
        
        std::deque<std::unique_ptr<Processor>> processors;
        std::map<std::string, Processor*> index;
    };
}

#endif
