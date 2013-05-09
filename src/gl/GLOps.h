#ifndef _DAAFE110_B468_11E2_A4CE_206A8A22A96A
#define _DAAFE110_B468_11E2_A4CE_206A8A22A96A

#include "gl.inc.h"

namespace glstreamer_gl
{
#define GLSTREAMER_GL_DEF_OP(Name, IDType, genFunc, delFunc)\
    struct Name\
    {\
    public:\
        virtual ~Name() noexcept = default;\
        \
        typedef IDType ID;\
        \
        ID generateOne()\
        {\
            ID id;\
            gl_Call(genFunc(1, &id));\
            return id;\
        }\
        \
        void deleteOne(ID const id)\
        {\
            delFunc(1, &id);\
        }\
    };
    
    GLSTREAMER_GL_DEF_OP(TextureOp, GLuint, glGenTextures, glDeleteTextures);
    GLSTREAMER_GL_DEF_OP(FramebufferOp, GLuint, glGenFramebuffers, glDeleteFramebuffers);
    GLSTREAMER_GL_DEF_OP(BufferOp, GLuint, glGenBuffers, glDeleteBuffers);
    GLSTREAMER_GL_DEF_OP(VertexArrayOp, GLuint, glGenVertexArrays, glDeleteVertexArrays);
    GLSTREAMER_GL_DEF_OP(RenderbufferOp, GLuint, glGenRenderbuffers, glDeleteRenderbuffers);
}

#endif
