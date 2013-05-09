#include "GLDepthDisplayer.h"

#include "GLFrame.h"
#include "GLThread.h"
#include "GLUniform.h"
#include "GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_gl;

char const* fragSource =
"#version 120\n"
"uniform sampler2D depth;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(depth, gl_TexCoord[0].st).rrrr;\n"
"}\n"
;

GLDepthDisplayer::GLDepthDisplayer():
program(),
shader(GL_FRAGMENT_SHADER)
{
    shader.addSourceStr(fragSource).compile();
    program.attachShader(shader).link();
    program.use();
    //GLUniform(program, "depth").set(0);
    program.useNone();
    
    inputArgs.addSlot<GLFrameData<DepthFrame>>("input");
}

void GLDepthDisplayer::run()
{
    GLTextureData<DepthFrame>& texDepth = inputArg(0).toSlot().local<GLTextureData<DepthFrame>>();
    
    auto width = texDepth.width(), height = texDepth.height();
    
    gl_Call(glViewport(0, 0, width, height));
    
    gl_Call(glMatrixMode(GL_MODELVIEW));
    gl_Call(glLoadIdentity());
    gl_Call(glMatrixMode(GL_PROJECTION));
    gl_Call(glLoadIdentity());
    gl_Call(glOrtho(0, width, 0, height, -1.0, 1.0));
    
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    
    gl_Call(glDisable(GL_DEPTH_TEST));
    gl_Call(glActiveTexture(GL_TEXTURE0));
    texDepth.setNearestFilter();
    program.use();
    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        glVertex2d(0.0, 0.0);
        
        glTexCoord2d(1.0, 0.0);
        glVertex2d(width, 0.0);
        
        glTexCoord2d(1.0, 1.0);
        glVertex2d(width, height);
        
        glTexCoord2d(0.0, 1.0);
        glVertex2d(0.0, height);
    gl_Call(glEnd());
    program.useNone();
    GLThread::getGLContextBinding<GLWindowBinding>()->swapBuffers();
}
