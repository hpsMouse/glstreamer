#ifndef _7855F1DA_B56C_11E2_ADA7_206A8A22A96A
#define _7855F1DA_B56C_11E2_ADA7_206A8A22A96A

#include <vector>

#include <assimp/scene.h>

namespace glstreamer_gl
{
    void drawScene(aiScene const* scene);
    void drawMesh(aiMesh const* mesh);
    
    void transformIndex(aiMesh const* mesh, std::vector<unsigned>& indices);
    void drawMeshWithIndex(aiMesh const* mesh, std::vector<unsigned> const& indices);
}

#endif
