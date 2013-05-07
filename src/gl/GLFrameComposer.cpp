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

GLFrameComposer::GLFrameComposer ( std::size_t nFrames ):
GLFrameComposer(nFrames, makeTrivialFrameList(nFrames), nFrames, nFrames)
{}

GLFrameComposer::GLFrameComposer ( std::size_t nFrames, const GLFrameComposer::ColorOnly& ):
GLFrameComposer(nFrames, makeColorOnlyFrameList(nFrames), nFrames, 0)
{}

GLFrameComposer::GLFrameComposer ( std::size_t nFrames, std::vector< GLFrameInfo >&& frameInfo, std::size_t colorFrames, std::size_t depthFrames ):
nFrames(nFrames),
frameInfo(std::move(frameInfo)),
colorFrames(colorFrames),
depthFrames(depthFrames),
fbo(),
program(),
shader(GL_FRAGMENT_SHADER)
{
    inputArgs._addSlot<GLViewport>("canvas_viewport_in");
    
    for(std::size_t i = 0; i < nFrames; ++i)
        inputArgs._addSlot<GLViewport>(genName("frame_viewport", i));
    
    for(std::size_t i = 0; i < colorFrames; ++i)
        inputArgs._addSlot<GLFrameData<RGBAFrame>>(genName("color_in", i));
    
    for(std::size_t i = 0; i < depthFrames; ++i)
        inputArgs._addSlot<GLFrameData<DepthFrame>>(genName("depth_in", i));
    
    inputArgs.refreshSimpleSlots();
    
    outputArgs.addSlots<GLViewport, GLFrameData<RGBAFrame>, GLFrameData<DepthFrame>>("canvas_viewport_out", "color_out", "depth_out");
    
    shader.addSourceStr(ComposeShader).compile();
    program.attachShader(shader).link();
    program.use();
    GLUniform(program, "color").set(0);
    GLUniform(program, "depth").set(1);
    program.useNone();
}

std::vector< GLFrameInfo > GLFrameComposer::makeTrivialFrameList ( std::size_t nFrames )
{
    std::vector<GLFrameInfo> result;
    result.reserve(nFrames);
    
    for(std::size_t i = 0; i < nFrames; ++i)
        result.push_back(GLFrameInfo({int(i), int(i)}));
    
    return result;
}

std::vector< GLFrameInfo > GLFrameComposer::makeColorOnlyFrameList ( std::size_t nFrames )
{
    std::vector<GLFrameInfo> result;
    result.reserve(nFrames);
    
    for(std::size_t i = 0; i < nFrames; ++i)
        result.push_back(GLFrameInfo({int(i), -1}));
    
    return result;
}

GLViewport& GLFrameComposer::canvasViewportIn()
{
    return inputArg("canvas_viewport_in").toSlot().arg<GLViewport>();
}

GLViewport& GLFrameComposer::canvasViewportOut()
{
    return outputArg("canvas_viewport_out").toSlot().arg<GLViewport>();
}

GLViewport& GLFrameComposer::frameViewport ( std::ptrdiff_t index )
{
    return inputArg("frame_viewport", index).toSlot().arg<GLViewport>();
}

GLTextureData< RGBAFrame >& GLFrameComposer::inputColorTexture ( std::ptrdiff_t index )
{
    return inputArg("color_in", index).toSlot().local<GLTextureData<RGBAFrame>>();
}

GLTextureData< DepthFrame >& GLFrameComposer::inputDepthTexture ( std::ptrdiff_t index )
{
    return inputArg("depth_in", index).toSlot().local<GLTextureData<DepthFrame>>();
}

GLTextureData< RGBAFrame >& GLFrameComposer::outputColorTexture()
{
    return outputArg("color_out").toSlot().local<GLTextureData<RGBAFrame>>();
}

GLTextureData< DepthFrame >& GLFrameComposer::outputDepthTexture()
{
    return outputArg("depth_out").toSlot().local<GLTextureData<DepthFrame>>();
}

void GLFrameComposer::run()
{
    GLViewport const& viewport = canvasViewportIn();
    canvasViewportOut() = viewport;
    
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
        auto const& frameVP = frameViewport(i);
        
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
                glVertex2d(frameVP.x, frameVP.y);
                
                glTexCoord2d(1.0, 0.0);
                glVertex2d(frameVP.x + frameVP.width, frameVP.y);
                
                glTexCoord2d(1.0, 1.0);
                glVertex2d(frameVP.x + frameVP.width, frameVP.y + frameVP.height);
                
                glTexCoord2d(0.0, 1.0);
                glVertex2d(frameVP.x, frameVP.y + frameVP.height);
            gl_Call(glEnd());
        }
    }
    
    program.useNone();
}
