#include <cstdio>

#include <utility>

#include "GLFrameComposer.h"

#include "GLFrame.h"
#include "GLUniform.h"

using namespace glstreamer;
using namespace glstreamer_gl;

static char const* ComposeShader =
"#version 120\n"
"uniform sampler2D color, depth;\n"
"uniform int hasColor, hasDepth;\n"
"void main()\n"
"{\n"
"    if(hasColor == 1)\n"
"        gl_FragColor = texture2D(color, gl_TexCoord[0].st);\n"
"    if(hasDepth == 1)\n"
"        gl_FragDepth = texture2D(depth, gl_TexCoord[0].st).r;\n"
"}\n"
;

GLFrameComposer::GLFrameComposer ( std::size_t nFrames, std::vector< GLFrameInfo >&& frameInfo, std::size_t colorFrames, std::size_t depthFrames ):
nFrames(nFrames),
frameInfo(std::move(frameInfo)),
colorFrames(colorFrames),
depthFrames(depthFrames),
fbo(),
program(),
shader(GL_FRAGMENT_SHADER)
{
    inputArgs._addSlot<GLViewport>("iviewport");
    for(std::size_t i = 0; i < nFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "viewport_%zu", i);
        inputArgs._addSlot<GLViewport>(name);
    }
    for(std::size_t i = 0; i < colorFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "icolor_%zu", i);
        inputArgs._addSlot<GLFrameData<RGBAFrame>>(name);
    }
    for(std::size_t i = 0; i < depthFrames; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "idepth_%zu", i);
        inputArgs._addSlot<GLFrameData<DepthFrame>>(name);
    }
    inputArgs.refreshSimpleSlots();
    
    outputArgs.addSlots<GLViewport, GLFrameData<RGBAFrame>, GLFrameData<DepthFrame>>("oviewport", "ocolor", "odepth");
    
    shader.addSourceStr(ComposeShader).compile();
    program.attachShader(shader).link();
    program.use();
    GLUniform(program, "color").set(0);
    GLUniform(program, "depth").set(1);
    program.useNone();
}

GLViewport& GLFrameComposer::getViewport()
{
    return inputArgs.externalArg<GLViewport>(0);
}

GLViewport& GLFrameComposer::viewportInput ( std::ptrdiff_t index )
{
    return inputArgs.externalArg<GLViewport>(1 + index);
}

GLTextureData< RGBAFrame >& GLFrameComposer::inputColorTexture ( std::ptrdiff_t index )
{
    return inputArgs.localArg<GLTextureData<RGBAFrame>>(1 + nFrames + index);
}

GLTextureData< DepthFrame >& GLFrameComposer::inputDepthTexture ( std::ptrdiff_t index )
{
    return inputArgs.localArg<GLTextureData<DepthFrame>>(1 + nFrames + colorFrames + index);
}

GLViewport& GLFrameComposer::getOutputViewport()
{
    return outputArgs.externalArg<GLViewport>(0);
}

GLTextureData< RGBAFrame >& GLFrameComposer::outputColorTexture()
{
    return outputArgs.localArg<GLTextureData<RGBAFrame>>(1);
}

GLTextureData< DepthFrame >& GLFrameComposer::outputDepthTexture()
{
    return outputArgs.localArg<GLTextureData<DepthFrame>>(2);
}

void GLFrameComposer::run()
{
    GLViewport const& viewport = getViewport();
    getOutputViewport() = viewport;
    
    auto width = viewport.width, height = viewport.height;
    
    auto& oColor = outputColorTexture();
    auto& oDepth = outputDepthTexture();
    
    oColor.resize(width, height);
    oDepth.resize(width, height);
    
    gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
    gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, oColor.obj(), 0));
    gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, oDepth.obj(), 0));
    
    gl_Call(glViewport(viewport.x, viewport.y, viewport.width, viewport.height));
    
    gl_Call(glMatrixMode(GL_MODELVIEW));
    gl_Call(glLoadIdentity());
    
    gl_Call(glMatrixMode(GL_PROJECTION));
    gl_Call(glLoadIdentity());
    gl_Call(glOrtho(0, width, 0, height, -1.0, 1.0));
    
    gl_Call(glClearColor(0.0, 0.0, 0.0, 0.0));
    gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    
    program.use();
    
    for(std::size_t i = 0; i < nFrames; ++i)
    {
        GLFrameInfo const& frame = frameInfo[i];
        auto const& frameViewport = viewportInput(i);
        
        GLTextureData<RGBAFrame> *iColor = nullptr;
        GLTextureData<DepthFrame> *iDepth = nullptr;
        
        if(frame.colorIndex >= 0)
            iColor = &inputColorTexture(frame.colorIndex);
        if(frame.depthIndex >= 0)
            iDepth = &inputDepthTexture(frame.depthIndex);
        
        if(iColor)
        {
            gl_Call(glActiveTexture(GL_TEXTURE0));
            gl_Call(glBindTexture(GL_TEXTURE_2D, iColor->obj()));
            
            gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            
            GLUniform(program, "hasColor").set(1);
        }
        else
            GLUniform(program, "hasColor").set(0);
        
        if(iDepth)
        {
            gl_Call(glActiveTexture(GL_TEXTURE1));
            gl_Call(glBindTexture(GL_TEXTURE_2D, iDepth->obj()));
            
            gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            gl_Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
            
            gl_Call(glEnable(GL_DEPTH_TEST));
            GLUniform(program, "hasDepth").set(1);
        }
        else
        {
            gl_Call(glDisable(GL_DEPTH_TEST));
            GLUniform(program, "hasDepth").set(0);
        }
        
        if(iColor || iDepth)
        {
            glBegin(GL_QUADS);
                glTexCoord2d(0.0, 0.0);
                glVertex2d(frameViewport.x, frameViewport.y);
                
                glTexCoord2d(1.0, 0.0);
                glVertex2d(frameViewport.x + frameViewport.width, frameViewport.y);
                
                glTexCoord2d(1.0, 1.0);
                glVertex2d(frameViewport.x + frameViewport.width, frameViewport.y + frameViewport.height);
                
                glTexCoord2d(0.0, 1.0);
                glVertex2d(frameViewport.x, frameViewport.y + frameViewport.height);
            gl_Call(glEnd());
        }
    }
    
    program.useNone();
}
