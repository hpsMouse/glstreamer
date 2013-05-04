#include <fstream>
#include <iterator>
#include <string>

#include "GLShader.h"

using namespace glstreamer_gl;

GLShader::GLShader ( GLenum shaderType ):
shader(glCreateShader(shaderType))
{
    if(shader == 0)
        throw GLException("glCreateShader(shaderType)", glGetError());
}

GLShader::~GLShader() noexcept
{
    glDeleteShader(shader);
}

GLShader& GLShader::addSource ( const char* filepath )
{
    std::ifstream file(filepath);
    std::string source(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
    
    return addSourceStr(source.c_str());
}

GLShader& GLShader::addSourceStr ( const char* source )
{
    gl_Call(glShaderSource(shader, 1, &source, nullptr));
    return *this;
}

GLShader& GLShader::compile()
{
    gl_Call(glCompileShader(shader));
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        GLchar log[1024];
        GLsizei logLen = 0;
        gl_Call(glGetShaderInfoLog(shader, 1024, &logLen, log));
        throw GLException("GLShader::compile()", std::string(log, log + logLen));
    }
    return *this;
}
