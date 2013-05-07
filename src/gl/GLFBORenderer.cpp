#include "GLFBORenderer.h"

#include "GLThread.h"
#include "GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLFBORenderer::GLFBORenderer ( std::size_t inputColorFrames, std::size_t inputDepthFrames, bool hasColorOutput, bool hasDepthOutput, bool show, ProjectionStyle projection ):
GLGenericRenderer(inputColorFrames,
                  inputDepthFrames,
                  hasColorOutput ? 1 : 0,
                  hasDepthOutput ? 1 : 0,
                  projection
                 ),
fbo(),
hasColor(hasColorOutput),
hasDepth(hasDepthOutput),
show(show)
{
}

void GLFBORenderer::setupOutputFramebuffer()
{
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
    auto const& viewport = this->getViewport();
    if(hasColor)
    {
        auto& colorTexture = this->getOutputColorFrame(0);
        colorTexture.resize(viewport.width, viewport.height);
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture.obj(), 0));
    }
    if(hasDepth)
    {
        auto& depthTexture = this->getOutputDepthFrame(0);
        depthTexture.resize(viewport.width, viewport.height);
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.obj(), 0));
    }
}

void GLFBORenderer::drawFinish()
{
    if(show && hasColor)
    {
        gl_Call(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
        gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        auto const& viewport = this->getViewport();
        auto width = viewport.width, height = viewport.height;
        gl_Call(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
        GLThread::getGLContextBinding<GLWindowBinding>()->swapBuffers();
    }
}
