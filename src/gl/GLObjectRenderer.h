#ifndef _752D3224_B711_11E2_9E51_206A8A22A96A
#define _752D3224_B711_11E2_9E51_206A8A22A96A

#include <vector>

#include "GLFBORenderer.h"

#include "GLMeshBuffer.h"

namespace glstreamer_gl
{
    class GLObjectRenderer : public GLFBORenderer
    {
    public:
        GLObjectRenderer(char const* objectPath,
                         char const* texturePath,
                         bool show = true,
                         ProjectionStyle projection = ProjectionStyle::Frustum
                        );
        
    private:
        virtual void draw();
        
        std::vector<GLMeshBuffer> meshes;
        TextureObject texture;
    };
}

#endif
