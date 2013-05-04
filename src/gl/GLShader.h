#ifndef _4428A774_B47F_11E2_B22E_206A8A22A96A
#define _4428A774_B47F_11E2_B22E_206A8A22A96A

#include "gl.inc.h"

namespace glstreamer_gl
{
    class GLShader
    {
    public:
        GLShader(GLenum shaderType);
        ~GLShader() noexcept;
        
        GLShader& addSource(char const* filepath);
        GLShader& addSourceStr(char const* source);
        GLShader& compile();
        
        GLuint get() const
        {
            return shader;
        }
        
        operator GLuint() const
        {
            return get();
        }
        
    private:
        GLShader(GLShader const&) = delete;
        GLShader& operator = (GLShader const&) = delete;
        
        GLuint shader;
    };
}

#endif
