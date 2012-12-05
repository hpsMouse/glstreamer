#ifndef _1087586C_2680_11E2_8DD5_206A8A22A96A
#define _1087586C_2680_11E2_8DD5_206A8A22A96A

#include <stdexcept>

namespace glstreamer
{
    class UnsupportedOperation : public std::logic_error
    {
    public:
        explicit UnsupportedOperation(const std::string& operation):
        logic_error("Unsupported operation: " + operation)
        {}
    };
    
    class NotFound : public std::runtime_error
    {
    public:
        explicit NotFound(const std::string& __arg):
        runtime_error("Not found: " + __arg)
        {}
    };
    
    class DefinationConflict : public std::runtime_error
    {
    public:
        explicit DefinationConflict(const std::string& __arg):
        runtime_error("Defination conflict: " + __arg)
        {}
    };
    
    class ResourceConflict : public std::runtime_error
    {
    public:
        explicit ResourceConflict ( const std::string& __arg ):
        runtime_error("Resource conflict: " + __arg)
        {}
    };
    
    class InternalError : public std::logic_error
    {
    public:
        explicit InternalError ( const std::string& __arg ):
        logic_error("Internal error: " + __arg)
        {}
    };
}

#endif
