#ifndef _E0089B32_B465_11E2_9B55_206A8A22A96A
#define _E0089B32_B465_11E2_9B55_206A8A22A96A

#include "GLEWContextBinding.h"

#include "GLException.h"

namespace glstreamer_gl
{
    inline void gl_TestError(char const* call = nullptr)
    {
        GLenum err = glGetError();
        if(err != GL_NO_ERROR)
            throw GLException(call, err);
    }
    
#define gl_Call(call)\
do\
{\
    call;\
    gl_TestError(#call);\
}\
while(0)
}

#endif
