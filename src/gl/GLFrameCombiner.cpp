#include <cstdio>

#include "GLFrameCombiner.h"

#include "GLFrame.h"
#include "GLUniform.h"

using namespace glstreamer;
using namespace glstreamer_gl;

static char const* ComposeShader =
"#version 120\n"
"uniform sampler2D color, depth;\n"
"void main()\n"
"{\n"
"    vec4 colorValue = texture2D(color, gl_TexCoord[0].st);\n"
"    float depthValue = texture2D(depth, gl_TexCoord[0].st).r;\n"
"    gl_FragColor = colorValue;\n"
"    gl_FragDepth = depthValue;\n"
"}\n"
;

GLFrameCombiner::GLFrameCombiner ( std::size_t nFrames, unsigned int width, unsigned int height ):
Processor(),
nFrames(nFrames),
width(width),
height(height),
fbo(),
program(),
shader(GL_FRAGMENT_SHADER)
{
    for(std::size_t i = 0; i < nFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "icolor_%zu", i);
        inputArgs._addSlot<GLFrameData<RGBAFrame>>(name);
    }
    for(std::size_t i = 0; i < nFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "idepth_%zu", i);
        inputArgs._addSlot<GLFrameData<DepthFrame>>(name);
    }
    inputArgs.refreshSimpleSlots();
    outputArgs.addSlots<GLFrameData<RGBAFrame>, GLFrameData<DepthFrame>>("ocolor", "odepth");
    
    shader.addSourceStr(ComposeShader).compile();
    program.attachShader(shader).link();
    program.use();
    GLUniform(program, "color").set(0);
    GLUniform(program, "depth").set(1);
    program.useNone();
}

void GLFrameCombiner::run()
{
    gl_Call(glViewport(0, 0, width, height));
    
    gl_Call(glMatrixMode(GL_MODELVIEW));
    gl_Call(glLoadIdentity());
    
    gl_Call(glMatrixMode(GL_PROJECTION));
    gl_Call(glLoadIdentity());
    gl_Call(glOrtho(0, width, 0, height, -1.0, 1.0));
    
    auto& outputColor = outputArgs.fullSlot(0).localArg->getArg<GLTextureData<RGBAFrame>>();
    auto& outputDepth = outputArgs.fullSlot(1).localArg->getArg<GLTextureData<DepthFrame>>();
    
    outputColor.resize(width, height);
    outputDepth.resize(width, height);
    
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
    gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputColor.obj(), 0));
    gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, outputDepth.obj(), 0));
    
    gl_Call(glClearColor(0.0, 0.0, 0.0, 0.0));
    gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    
    program.use();
    
    gl_Call(glEnable(GL_DEPTH_TEST));
    
    for(std::size_t i = 0; i < nFrames; ++i)
    {
        auto& inputColor = inputArgs.fullSlot(i).localArg->getArg<GLTextureData<RGBAFrame>>();
        auto& inputDepth = inputArgs.fullSlot(nFrames + i).localArg->getArg<GLTextureData<DepthFrame>>();
        
        gl_Call(glActiveTexture(GL_TEXTURE0));
        gl_Call(glBindTexture(GL_TEXTURE_2D, inputColor.obj()));
        
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        
        gl_Call(glActiveTexture(GL_TEXTURE1));
        gl_Call(glBindTexture(GL_TEXTURE_2D, inputDepth.obj()));
        
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
        
        auto inputWidth = inputColor.width();
        auto inputHeight = inputColor.height();
        
        glBegin(GL_QUADS);
            glTexCoord2d(0.0, 0.0);
            glVertex2d(0.0, 0.0);
            
            glTexCoord2d(1.0, 0.0);
            glVertex2d(inputWidth, 0.0);
            
            glTexCoord2d(1.0, 1.0);
            glVertex2d(inputWidth, inputHeight);
            
            glTexCoord2d(0.0, 1.0);
            glVertex2d(0.0, inputHeight);
        gl_Call(glEnd());
    }
    
    program.useNone();
}
