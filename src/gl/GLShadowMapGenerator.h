#ifndef _13457D58_B862_11E2_9A87_206A8A22A96A
#define _13457D58_B862_11E2_9A87_206A8A22A96A

#include <vector>

#include "../processor.h"

#include "GLFrame.h"
#include "GLMeshBuffer.h"
#include "GLObject.h"
#include "parameter_types.h"

namespace glstreamer_gl
{
    /**
     * Inputs: [ObjectState], [Viewport]. [DataRange]
     * Outputs: [DepthImage], [TexCoordMatrix]
     */
    class GLShadowMapGenerator : public glstreamer::Processor
    {
    public:
        GLShadowMapGenerator(char const* objectPath, std::size_t numLightSources, std::vector<ProjectionStyle>&& lightProjections);
        
    private:
        std::size_t numLightSources;
        std::vector<ProjectionStyle> lightProjections;
        
        FramebufferObject fbo;
        std::vector<GLMeshBuffer> meshes;
        
        virtual void run() override;
    };
}

#endif
