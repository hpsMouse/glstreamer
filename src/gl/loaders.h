#ifndef _356AAC20_B586_11E2_9FA9_206A8A22A96A
#define _356AAC20_B586_11E2_9FA9_206A8A22A96A

#include <vector>

#include "GLMeshBuffer.h"
#include "GLObject.h"

namespace glstreamer_gl
{
    void loadMeshes(char const* path, std::vector<GLMeshBuffer>& meshes);
    TextureObject load2DTexture(char const* path);
}

#endif
