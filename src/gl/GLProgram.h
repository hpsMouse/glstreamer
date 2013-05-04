#ifndef _622D2A36_B481_11E2_918F_206A8A22A96A
#define _622D2A36_B481_11E2_918F_206A8A22A96A

#include "gl.inc.h"

#include "types.h"

namespace glstreamer_gl
{
    class GLProgram
    {
    public:
        GLProgram();
        ~GLProgram() noexcept;
        
        GLProgram& attachShader(GLShader &shader);
        GLProgram& link();
        
        GLProgram& use();
        static void useNone();
        
        GLuint get() const
        {
            return program;
        }
        
        operator GLuint () const
        {
            return get();
        }
        
    private:
        GLProgram(GLProgram const&) = delete;
        GLProgram& operator = (GLProgram const&) = delete;
        
        GLuint program;
    };
}

#endif
