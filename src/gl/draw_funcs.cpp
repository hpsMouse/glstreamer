#include <cstdio>

#include "gl.inc.h"

#include "draw_funcs.h"

void glstreamer_gl::drawScene ( const aiScene* scene )
{
    for(unsigned idxMesh = 0; idxMesh < scene->mNumMeshes; ++idxMesh)
        drawMesh(scene->mMeshes[idxMesh]);
}

void glstreamer_gl::drawMesh ( const aiMesh* mesh )
{
    gl_Call(glEnableClientState(GL_VERTEX_ARRAY));
    gl_Call(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
    {
        std::fputs("Currently only triangles are supported.\n", stderr);
        return;
    }
    gl_Call(glVertexPointer(3, GL_FLOAT, 0, mesh->mVertices));
    gl_Call(glTexCoordPointer(3, GL_FLOAT, 0, mesh->mTextureCoords[0]));
    for(unsigned idxFace = 0; idxFace < mesh->mNumFaces; ++idxFace)
    {
        aiFace const& face = mesh->mFaces[idxFace];
        glDrawElements(GL_TRIANGLES, face.mNumIndices, GL_UNSIGNED_INT, face.mIndices);
    }
}

void glstreamer_gl::transformIndex ( const aiMesh* mesh, std::vector< unsigned int >& indices )
{
    indices.clear();
    
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
    {
        fputs("Error: Only triangles can be transformed.\n", stderr);
        return;
    }
    
    indices.reserve(mesh->mNumFaces * 3);
    
    for(unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace const& face = mesh->mFaces[i];
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

void glstreamer_gl::drawMeshWithIndex ( const aiMesh* mesh, const std::vector< unsigned int >& indices )
{
    gl_Call(glEnableClientState(GL_VERTEX_ARRAY));
    gl_Call(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    gl_Call(glVertexPointer(3, GL_FLOAT, 0, mesh->mVertices));
    gl_Call(glTexCoordPointer(3, GL_FLOAT, 0, mesh->mTextureCoords[0]));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
}
