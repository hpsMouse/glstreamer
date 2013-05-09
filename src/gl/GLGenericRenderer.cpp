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
    inputArgs._addSlot<GLViewport>("viewport_in");
    inputArgs._addSlot<GLDataRange>("range");
    outputArgs._addSlot<GLViewport>("viewport_out");
    
    for(std::size_t i = 0; i < inputColorFrames; ++i)
        inputArgs._addSlot<GLFrameData<RGBAFrame>>(genName("color_in", i));
    
    for(std::size_t i = 0; i < inputDepthFrames; ++i)
        inputArgs._addSlot<GLFrameData<DepthFrame>>(genName("depth_in", i));
    
    for(std::size_t i = 0; i < outputColorFrames; ++i)
        outputArgs._addSlot<GLFrameData<RGBAFrame>>(genName("color_out", i));
    
    for(std::size_t i = 0; i < outputDepthFrames; ++i)
        outputArgs._addSlot<GLFrameData<DepthFrame>>(genName("depth_out", i));
    
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
    return inputArg("color_in", index).toSlot().local<GLTextureData<RGBAFrame>>();
}

GLTextureData< DepthFrame >& GLGenericRenderer::getInputDepthFrame ( std::ptrdiff_t index )
{
    return inputArg("depth_in", index).toSlot().local<GLTextureData<DepthFrame>>();
}

GLTextureData< RGBAFrame >& GLGenericRenderer::getOutputColorFrame ( std::ptrdiff_t index )
{
    return outputArg("color_out", index).toSlot().local<GLTextureData<RGBAFrame>>();
}

GLTextureData< DepthFrame >& GLGenericRenderer::getOutputDepthFrame ( std::ptrdiff_t index )
{
    return outputArg("depth_out", index).toSlot().local<GLTextureData<DepthFrame>>();
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
    applyViewport(viewport, projection);
    
    for(std::size_t i = 0; i < inputColorFrames; ++i)
    {
        auto unitId = getInputColorTexture(i);
        gl_Call(glActiveTexture(GL_TEXTURE0 + unitId));
        gl_Call(glBindTexture(GL_TEXTURE_2D, getInputColorFrame(i).obj()));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    
    for(std::size_t i = 0; i < inputDepthFrames; ++i)
    {
        auto unitId = getInputDepthTexture(i);
        gl_Call(glActiveTexture(GL_TEXTURE0 + unitId));
        gl_Call(glBindTexture(GL_TEXTURE_2D, getInputDepthFrame(i).obj()));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }
    
    getViewportOutput() = viewport;
    
    draw();
    
    drawFinish();
}
