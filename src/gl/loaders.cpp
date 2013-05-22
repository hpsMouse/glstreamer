#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <FreeImagePlus.h>

#include "loaders.h"

#include "GLException.h"

static void transformIndex ( const aiMesh* mesh, std::vector< unsigned int >& indices )
{
    using namespace glstreamer_gl;
    
    indices.clear();
    
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
        throw GLException("transformIndex", "Only triangles can be transformed.");
    
    indices.reserve(mesh->mNumFaces * 3);
    
    for(unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace const& face = mesh->mFaces[i];
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

void glstreamer_gl::loadMeshes ( const char* path, std::vector< glstreamer_gl::GLMeshBuffer >& meshes )
{
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
    
    meshes.clear();
    meshes.resize(scene->mNumMeshes);
    
    for(unsigned i = 0; i < scene->mNumMeshes; ++i)
    {
        std::vector<unsigned> indices;
        aiMesh const* mesh = scene->mMeshes[i];
        transformIndex(mesh, indices);
        GLMeshBuffer& meshBuffer = meshes[i];
        
        gl_Call(glBindVertexArray(meshBuffer.vao));
        
        gl_Call(glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.vertexBuffer));
        gl_Call(glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mVertices, GL_STATIC_DRAW));
        gl_Call(glVertexPointer(3, GL_FLOAT, 0, 0));
        
        gl_Call(glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.texCoordBuffer));
        gl_Call(glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mTextureCoords[0], GL_STATIC_DRAW));
        gl_Call(glTexCoordPointer(3, GL_FLOAT, 0, 0));
        
        gl_Call(glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.normalBuffer));
        gl_Call(glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mNormals, GL_STATIC_DRAW));
        gl_Call(glNormalPointer(GL_FLOAT, 0, 0));
        
        gl_Call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.indexBuffer));
        gl_Call(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW));
        
        meshBuffer.nIndices = indices.size();
    }
}

glstreamer_gl::TextureObject glstreamer_gl::load2DTexture ( const char* path )
{
    fipImage image;
    if(!image.load(path))
        throw GLException("image.load(path)", "Failed.");
    if(!image.convertTo32Bits())
        throw GLException("image.convertTo32Bits()", "Failed.");
    
    TextureObject texture;
    gl_Call(glBindTexture(GL_TEXTURE_2D, texture));
    gl_Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, image.accessPixels()));
    gl_Call(glGenerateMipmap(GL_TEXTURE_2D));
    return texture;
}
