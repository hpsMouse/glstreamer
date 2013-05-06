#ifndef _BDD30E88_B584_11E2_B822_206A8A22A96A
#define _BDD30E88_B584_11E2_B822_206A8A22A96A

#include "GLObject.h"

namespace glstreamer_gl
{
    struct GLMeshBuffer
    {
        GLMeshBuffer(): nIndices(), vao(), vertexBuffer(), texCoordBuffer(), indexBuffer() {}
        
        GLMeshBuffer(GLMeshBuffer&&) = default;
        GLMeshBuffer& operator = (GLMeshBuffer&&) = default;
        
        void draw(double start = 0.0, double end = 1.0) const
        {
            GLsizei first = start * nIndices, last = end * nIndices;
            gl_Call(glBindVertexArray(vao));
            gl_Call(glEnableClientState(GL_VERTEX_ARRAY));
            gl_Call(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
            gl_Call(glDrawElements(GL_TRIANGLES, last - first, GL_UNSIGNED_INT, (unsigned*)nullptr + first));
        }
        
        unsigned nIndices;
        VertexArrayObject vao;
        BufferObject vertexBuffer;
        BufferObject texCoordBuffer;
        BufferObject indexBuffer;
        
    private:
        GLMeshBuffer(GLMeshBuffer const&) = delete;
        GLMeshBuffer& operator = (GLMeshBuffer const&) = delete;
    };
}

#endif
