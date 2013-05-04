#include "GLProgram.h"

#include "GLShader.h"

using namespace glstreamer_gl;

GLProgram::GLProgram():
program(glCreateProgram())
{
    if(program == 0)
        throw GLException("glCreateProgram()", glGetError());
}

GLProgram::~GLProgram() noexcept
{
    glDeleteProgram(program);
}

GLProgram& GLProgram::attachShader( GLShader& shader )
{
    gl_Call(glAttachShader(program, shader.get()));
    return *this;
}

GLProgram& GLProgram::link()
{
    gl_Call(glLinkProgram(program));
    
    GLint status;
    gl_Call(glGetProgramiv(program, GL_LINK_STATUS, &status));
    if(status != GL_TRUE)
    {
        GLchar log[1024];
        GLsizei logLen = 0;
        gl_Call(glGetProgramInfoLog(program, 1024, &logLen, log));
    }
    return *this;
}

GLProgram& GLProgram::use()
{
    gl_Call(glUseProgram(program));
    return *this;
}

void GLProgram::useNone()
{
    gl_Call(glUseProgram(0));
}
