#include "GLUniform.h"

#include "GLProgram.h"

using namespace glstreamer_gl;

GLUniform::GLUniform ( GLProgram& program, const GLchar* name ):
location(glGetUniformLocation(program.get(), name))
{
    gl_TestError("glGetUniformLocation(program.get(), name)");
    if(location == -1)
        throw GLException("GLUniform", name + std::string(" is not an active uniform."));
}

void GLUniform::set ( GLfloat v0 )
{
    gl_Call(glUniform1f(location, v0));
}

void GLUniform::set ( GLfloat v0, GLfloat v1 )
{
    gl_Call(glUniform2f(location, v0, v1));
}

void GLUniform::set ( GLfloat v0, GLfloat v1, GLfloat v2 )
{
    gl_Call(glUniform3f(location, v0, v1, v2));
}

void GLUniform::set ( GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
{
    gl_Call(glUniform4f(location, v0, v1, v2, v3));
}

void GLUniform::set ( GLint v0 )
{
    gl_Call(glUniform1i(location, v0));
}

void GLUniform::set ( GLint v0, GLint v1 )
{
    gl_Call(glUniform2i(location, v0, v1));
}

void GLUniform::set ( GLint v0, GLint v1, GLint v2 )
{
    gl_Call(glUniform3i(location, v0, v1, v2));
}

void GLUniform::set ( GLint v0, GLint v1, GLint v2, GLint v3 )
{
    gl_Call(glUniform4i(location, v0, v1, v2, v3));
}

void GLUniform::set ( GLuint v0 )
{
    gl_Call(glUniform1ui(location, v0));
}

void GLUniform::set ( GLuint v0, GLuint v1 )
{
    gl_Call(glUniform2ui(location, v0, v1));
}

void GLUniform::set ( GLuint v0, GLuint v1, GLuint v2 )
{
    gl_Call(glUniform3ui(location, v0, v1, v2));
}

void GLUniform::set ( GLuint v0, GLuint v1, GLuint v2, GLuint v3 )
{
    gl_Call(glUniform4ui(location, v0, v1, v2, v3));
}
