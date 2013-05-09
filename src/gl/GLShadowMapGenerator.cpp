#include "GLShadowMapGenerator.h"

#include "GLFrame.h"

#include "loaders.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLShadowMapGenerator::GLShadowMapGenerator ( const char* objectPath, std::size_t numLightSources, std::vector< ProjectionStyle >&& lightProjections ):
numLightSources(numLightSources),
lightProjections(std::move(lightProjections)),
fbo(),
meshes()
{
    loadMeshes(objectPath, meshes);
    
    for(std::size_t i = 0; i < numLightSources; ++i)
        inputArgs._addSlot<GLObjectState>(genName("state", i));
    
    for(std::size_t i = 0; i < numLightSources; ++i)
        inputArgs._addSlot<GLViewport>(genName("viewport_in", i));
    
    for(std::size_t i = 0; i < numLightSources; ++i)
        inputArgs._addSlot<GLDataRange>(genName("range", i));
    
    for(std::size_t i = 0; i < numLightSources; ++i)
        outputArgs._addSlot<GLFrameData<DepthFrame>>(genName("depth", i));
    
    for(std::size_t i = 0; i < numLightSources; ++i)
        outputArgs._addSlot<GLViewport>(genName("viewport_out", i));
    
    inputArgs.refreshSimpleSlots();
    outputArgs.refreshSimpleSlots();
}

void GLShadowMapGenerator::run()
{
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
    gl_Call(glEnable(GL_DEPTH_TEST));
    for(std::size_t i = 0; i < numLightSources; ++i)
    {
        GLObjectState const& state = inputArg("state", i).toSlot().arg<GLObjectState>();
        GLViewport const& viewport = inputArg("viewport_in", i).toSlot().arg<GLViewport>();
        GLDataRange const& range = inputArg("range", i).toSlot().arg<GLDataRange>();
        GLTextureData<DepthFrame>& texDepth = outputArg("depth", i).toSlot().local<GLTextureData<DepthFrame>>();
        GLViewport& viewportOut = outputArg("viewport_out", i).toSlot().arg<GLViewport>();
        
        viewportOut = viewport;
        
        auto width = viewport.width, height = viewport.height;
        texDepth.resize(width, height);
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texDepth.obj(), 0));
        
        gl_Call(glClear(GL_DEPTH_BUFFER_BIT));
        
        gl_Call(glViewport(0, 0, width, height));
        
        gl_Call(glMatrixMode(GL_PROJECTION));
        gl_Call(glLoadIdentity());
        
        gl_Call(glMatrixMode(GL_MODELVIEW));
        gl_Call(glLoadIdentity());
        applyViewport(viewport, lightProjections[i]);
        state.apply();
        
        for(GLMeshBuffer const& mesh : meshes)
            mesh.draw(range.start, range.end);
    }
}
