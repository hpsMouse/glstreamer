#ifndef _E6D41DDC_960C_11E2_88A2_206A8A22A96A
#define _E6D41DDC_960C_11E2_88A2_206A8A22A96A

#include <map>
#include <memory>
#include <string>

#include <boost/any.hpp>

#include "processor.h"

namespace glstreamer
{
    class ProcessorManager
    {
    public:
        typedef std::unique_ptr<Processor> (*ProcessorMaker)(std::map<std::string, boost::any> const& args);
        
        /**
         * \brief Register a new type of processor. NOT thread-safe.
         */
        static void registerProcessor(std::string const& name, ProcessorMaker maker);
        
        static ProcessorMaker findProcessor(std::string const& name);
        static std::unique_ptr<Processor> makeProcessor(std::string const& name, std::map<std::string, boost::any> const& args);
        
    private:
        ProcessorManager() = delete;
        
        static std::map<std::string, ProcessorMaker> makers;
    };
}

#endif
