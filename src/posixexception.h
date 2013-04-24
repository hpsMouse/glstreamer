#ifndef _70D10618_3B8F_11E2_AFBB_206A8A22A96A
#define _70D10618_3B8F_11E2_AFBB_206A8A22A96A

#include <cerrno>

#include <stdexcept>

namespace glstreamer
{
    class PosixException : public std::runtime_error
    {
    public:
        PosixException(std::string const& funcName, int errnoValue, bool backtrace = false);
        
        std::string const& getFuncName() { return call; }
        int getErrno() { return errnoValue; }
        
        virtual const char* what() const noexcept override;
        
    private:
        std::string message;
        std::string call;
        int errnoValue;
        std::string backtrace;
    };
}

#define throw_posix(call) throw ::glstreamer::PosixException(::std::string(__PRETTY_FUNCTION__) + ":" + #call, errno)

#endif
