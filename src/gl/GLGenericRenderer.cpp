#include <cstdio>

#include "gl.inc.h"

#include "GLGenericRenderer.h"

#include "GLThread.h"
#include "GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLGenericRenderer::GLGenericRenderer ( std::size_t inputColorFrames, std::size_t inputDepthFrames, std::size_t outputColorFrames, std::size_t outputDepthFrames, ProjectionStyle projection ):
inputColorFrames(inputColorFrames),
inputDepthFrames(inputDepthFrames),
outputColorFrames(outputColorFrames),
outputDepthFrames(outputDepthFrames),
projection(projection)
{
    inputArgs._addSlot<GLViewport>("viewport");
    for(std::size_t i = 0; i < inputColorFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "icolor_%zu", i);
        inputArgs._addSlot<GLFrameData<RGBAFrame>>(name);
    }
    for(std::size_t i = 0; i < inputDepthFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "idepth_%zu", i);
        inputArgs._addSlot<GLFrameData<DepthFrame>>(name);
    }
    for(std::size_t i = 0; i < outputColorFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "ocolor_%zu", i);
        outputArgs._addSlot<GLFrameData<RGBAFrame>>(name);
    }
    for(std::size_t i = 0; i < outputDepthFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "odepth_%zu", i);
        outputArgs._addSlot<GLFrameData<DepthFrame>>(name);
    }
    inputArgs.refreshSimpleSlots();
    outputArgs.refreshSimpleSlots();
}

void GLGenericRenderer::setupOutputFramebuffer()
{
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void GLGenericRenderer::drawFinish()
{
    GLThread::getGLContextBinding<GLWindowBinding>()->swapBuffers();
}

GLTextureData< RGBAFrame >& GLGenericRenderer::getInputColorFrame ( std::ptrdiff_t index )
{
    return inputArgs.fullSlot(fixedInputs + index).localArg->getArg<GLTextureData<RGBAFrame>>();
}

GLTextureData< DepthFrame >& GLGenericRenderer::getInputDepthFrame ( std::ptrdiff_t index )
{
    return inputArgs.fullSlot(fixedInputs + inputColorFrames + index).localArg->getArg<GLTextureData<DepthFrame>>();
}

GLTextureData< RGBAFrame >& GLGenericRenderer::getOutputColorFrame ( std::ptrdiff_t index )
{
    return outputArgs.fullSlot(fixedOutputs + index).localArg->getArg<GLTextureData<RGBAFrame>>();
}

GLTextureData< DepthFrame >& GLGenericRenderer::getOutputDepthFrame ( std::ptrdiff_t index )
{
    return outputArgs.fullSlot(fixedOutputs + outputColorFrames + index).localArg->getArg<GLTextureData<DepthFrame>>();
}

int GLGenericRenderer::getInputColorTexture ( std::ptrdiff_t index )
{
    return index;
}

int GLGenericRenderer::getInputDepthTexture ( std::ptrdiff_t index )
{
    return inputColorFrames + index;
}

void GLGenericRenderer::run()
{
    setupOutputFramebuffer();
    
    GLViewport const& viewport = *static_cast<GLViewport*>(inputArgs.simpleSlot(0).arg);
    gl_Call(glViewport(0, 0, viewport.width, viewport.height));
    gl_Call(glMatrixMode(GL_PROJECTION));
    gl_Call(glLoadIdentity());
    switch(projection)
    {
        case ProjectionStyle::Frustum:
            gl_Call(glFrustum(viewport.left, viewport.right, viewport.bottom, viewport.top, viewport.near, viewport.far));
            break;
        case ProjectionStyle::Ortho:
        default:
            gl_Call(glOrtho(viewport.left, viewport.right, viewport.bottom, viewport.top, viewport.near, viewport.far));
            break;
    }
    
    for(std::size_t i = 0; i < inputColorFrames; ++i)
    {
        auto unitId = getInputColorTexture(i);
        gl_Call(glActiveTexture(GL_TEXTURE0 + unitId));
        gl_Call(glBindTexture(GL_TEXTURE_2D, getInputColorFrame(i).obj()));
        gl_Call(glGenerateMipmap(GL_TEXTURE_2D));
    }
    
    for(std::size_t i = 0; i < inputDepthFrames; ++i)
    {
        auto unitId = getInputDepthTexture(i);
        gl_Call(glActiveTexture(GL_TEXTURE0 + unitId));
        gl_Call(glBindTexture(GL_TEXTURE_2D, getInputDepthFrame(i).obj()));
        gl_Call(glGenerateMipmap(GL_TEXTURE_2D));
    }
    
    draw();
    
    drawFinish();
}
