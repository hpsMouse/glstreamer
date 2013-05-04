#ifndef _37E69700_B489_11E2_9177_206A8A22A96A
#define _37E69700_B489_11E2_9177_206A8A22A96A

#include <stdexcept>

#include "GLEWContextBinding.h"

namespace glstreamer_gl
{
    class GLException : public std::runtime_error
    {
    public:
        explicit GLException(char const* call, GLenum error);
        explicit GLException(char const* call, std::string const& msg );
    };
}

#endif
