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
    
    class DefinitionConflict : public std::runtime_error
    {
    public:
        explicit DefinitionConflict(const std::string& __arg):
        runtime_error("Definition conflict: " + __arg)
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
    
    class EOFError : public std::runtime_error
    {
    public:
        explicit EOFError ():
        runtime_error("Unexpected EOF.")
        {}
    };
    
    class ProtocolError : public std::logic_error
    {
    public:
        explicit ProtocolError ( const std::string& __arg ):
        logic_error("Protocol error: " + __arg)
        {}
    };
    
    #define wrap(content) (std::string(__PRETTY_FUNCTION__) + ": " + (content))
}

#endif
