#include <string>

#include "GLException.h"

using namespace glstreamer_gl;

static std::string makePrefix(char const* str)
{
    if(str != nullptr)
        return str + std::string(": ");
    else
        return std::string();
}

GLException::GLException ( const char* call, GLenum error ) : runtime_error(makePrefix(call) + (char const*)gluErrorString(error))
{}

GLException::GLException ( const char* call, const std::string& msg ) : runtime_error(makePrefix(call) + msg)
{}
