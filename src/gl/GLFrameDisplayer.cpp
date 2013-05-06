#include <cstdio>

#include "gl.inc.h"

#include "GLFrameDisplayer.h"

#include "GLFrame.h"
#include "GLThread.h"
#include "GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLFrameDisplayer::GLFrameDisplayer():
Processor(),
fbo()
{
    inputArgs.addSlot<GLFrameData<RGBAFrame>>("input");
}

void GLFrameDisplayer::run()
{
    auto& frame = inputArgs.fullSlot(0).localArg->getArg<GLTextureData<RGBAFrame>>();
    auto width = frame.width();
    auto height = frame.height();
    
    gl_Call(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
    gl_Call(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame.obj(), 0));
    
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    gl_Call(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    
    GLThread::getGLContextBinding<GLWindowBinding>()->swapBuffers();
}
