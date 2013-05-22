#ifndef _072FE0C0_BBA1_11E2_91E3_206A8A22A96A
#define _072FE0C0_BBA1_11E2_91E3_206A8A22A96A

#include "GLFBORenderer.h"
#include "GLMeshBuffer.h"
#include "GLObject.h"
#include "GLProgram.h"
#include "GLShader.h"
#include "GLUniform.h"

namespace glstreamer_gl
{
    /**
     * Input: Viewport, Range, ObjectState, [ShadowMap], [Matrix]
     * Output: Viewport, Color, Depth
     */
    class GLSimpleShadowRenderer : public GLFBORenderer
    {
    public:
        GLSimpleShadowRenderer(std::size_t nLights, char const* objPath, char const* texturePath, bool show = false);
        
    private:
        std::size_t nLights;
        std::vector<GLMeshBuffer> meshes;
        TextureObject texture;
        GLProgram program;
        GLShader vShader;
        GLShader fShader;
        
        GLUniform lightMatrix;
        
        virtual void draw() override;
        
        GLMatrix const& getMatrix(std::ptrdiff_t index);
    };
}

#endif
