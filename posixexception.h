#ifndef _70D10618_3B8F_11E2_AFBB_206A8A22A96A
#define _70D10618_3B8F_11E2_AFBB_206A8A22A96A

#include <stdexcept>

namespace glstreamer
{
    class PosixException : public std::runtime_error
    {
    public:
        PosixException(std::string const& funcName, int errnoValue);
        
        std::string const& getFuncName() { return call; }
        int getErrno() { return errnoValue; }
        
    private:
        std::string call;
        int errnoValue;
    };
}

#endif
