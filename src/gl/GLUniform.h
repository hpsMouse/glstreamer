#ifndef _69EA9F18_B483_11E2_9A63_206A8A22A96A
#define _69EA9F18_B483_11E2_9A63_206A8A22A96A

#include "gl.inc.h"

#include "types.h"

namespace glstreamer_gl
{
    class GLUniform
    {
    public:
        GLUniform(GLProgram& program, GLchar const* name);
        ~GLUniform() noexcept = default;
        
        void set(GLfloat v0);
        void set(GLfloat v0, GLfloat v1);
        void set(GLfloat v0, GLfloat v1, GLfloat v2);
        void set(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
        
        void set(GLint v0);
        void set(GLint v0, GLint v1);
        void set(GLint v0, GLint v1, GLint v2);
        void set(GLint v0, GLint v1, GLint v2, GLint v3);
        
        void set(GLuint v0);
        void set(GLuint v0, GLuint v1);
        void set(GLuint v0, GLuint v1, GLuint v2);
        void set(GLuint v0, GLuint v1, GLuint v2, GLuint v3);
        
    private:
        GLint location;
    };
}

#endif
