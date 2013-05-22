#ifndef _BDD30E88_B584_11E2_B822_206A8A22A96A
#define _BDD30E88_B584_11E2_B822_206A8A22A96A

#include "GLObject.h"

namespace glstreamer_gl
{
    struct GLMeshBuffer
    {
        GLMeshBuffer(): nIndices(), vao(), vertexBuffer(), texCoordBuffer(), normalBuffer(), indexBuffer() {}
        
        GLMeshBuffer(GLMeshBuffer&&) = default;
        GLMeshBuffer& operator = (GLMeshBuffer&&) = default;
        
        void draw(double start = 0.0, double end = 1.0) const
        {
            unsigned nFaces = nIndices / 3;
            GLsizei firstFace = start * nFaces, lastFace = end * nFaces;
            GLsizei first = firstFace * 3, last = lastFace * 3;
            gl_Call(glBindVertexArray(vao));
            gl_Call(glEnableClientState(GL_VERTEX_ARRAY));
            gl_Call(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
            gl_Call(glEnableClientState(GL_NORMAL_ARRAY));
            gl_Call(glDrawElements(GL_TRIANGLES, last - first, GL_UNSIGNED_INT, (unsigned*)nullptr + first));
        }
        
        unsigned nIndices;
        VertexArrayObject vao;
        BufferObject vertexBuffer;
        BufferObject texCoordBuffer;
        BufferObject normalBuffer;
        BufferObject indexBuffer;
        
    private:
        GLMeshBuffer(GLMeshBuffer const&) = delete;
        GLMeshBuffer& operator = (GLMeshBuffer const&) = delete;
    };
}

#endif
