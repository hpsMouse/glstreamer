#include <unistd.h>

#include <posixpp/PThreadBarrier.h>

#include "../gl/gl.inc.h"

#include "../glstreamer.h"
#include "../threadedlink.h"

#include "../gl/GLContextBinding.h"
#include "../gl/GLFixedProcessor.h"
#include "../gl/GLProgram.h"
#include "../gl/GLShader.h"
#include "../gl/GLThread.h"
#include "../gl/GLUniform.h"
#include "../gl/GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_gl;

class TextureFiller : public GLFixedProcessor<TypeList<>, TypeList<>, TypeList<>, TypeList<RGBAFrame, DepthFrame>>
{
public:
    TextureFiller():
    GLFixedProcessor<TypeList<>, TypeList<>, TypeList<>, TypeList<RGBAFrame, DepthFrame>>("ColorFrame", "DepthFrame"),
    fbo()
    {}
    
private:
    FramebufferObject fbo;
    
    void gl_run(GLTextureData<RGBAFrame>& color, GLTextureData<DepthFrame>& depth) override final
    {
        color.resize(600, 600);
        depth.resize(600, 600);
        
        gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
        
        gl_Call(glEnable(GL_DEPTH_TEST));
        
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.obj(), 0));
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.obj(), 0));
        gl_Call(glClearColor(0.0f, 0.0f, 0.5f, 1.0f));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        gl_Call(glViewport(0, 0, 600, 600));
        
        gl_Call(glMatrixMode(GL_PROJECTION));
        gl_Call(glLoadIdentity());
        gl_Call(glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 3.0));
        
        gl_Call(glMatrixMode(GL_MODELVIEW));
        gl_Call(glLoadIdentity());
        gl_Call(glTranslated(0.0, 0.0, -2.0));
        gl_Call(glRotated(30, 0.0, 1.0, 0.0));
        
        gl_Call(glColor4d(0.5, 0.5, 0.0, 1.0));
        glBegin(GL_QUADS);
            glVertex2d(-1.0, -1.0);
            glVertex2d( 1.0, -1.0);
            glVertex2d( 1.0,  1.0);
            glVertex2d(-1.0,  1.0);
        gl_Call(glEnd());
        
        gl_Call(glLoadIdentity());
        gl_Call(glTranslated(0.0, 0.0, -2.0));
        gl_Call(glRotated(-30, 0.0, 1.0, 0.0));
        
        gl_Call(glColor4d(0.5, 0.0, 0.5, 1.0));
        glBegin(GL_QUADS);
            glVertex2d(-1.0, -1.0);
            glVertex2d( 1.0, -1.0);
            glVertex2d( 1.0,  1.0);
            glVertex2d(-1.0,  1.0);
        gl_Call(glEnd());
        
        write(2, "Readback finished\n", sizeof("Readback finished\n") - 1);
    }
};

class FrameDisplayer : public GLFixedProcessor<TypeList<>, TypeList<>, TypeList<RGBAFrame, DepthFrame>, TypeList<>>
{
public:
    FrameDisplayer():
    GLFixedProcessor<TypeList<>, TypeList<>, TypeList<RGBAFrame, DepthFrame>, TypeList<>>("RedFrame", "BlueFrame"),
    fshader(GL_FRAGMENT_SHADER),
    program()
    {
        fshader.addSourceStr(fshaderSource).compile();
        program.attachShader(fshader).link();
    }
    
private:
    static constexpr char const* fshaderSource = 
    "#version 120\n"
    "uniform sampler2D color;\n"
    "uniform sampler2D depth;\n"
    "void main()\n"
    "{\n"
//    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "    gl_FragColor = texture2D(color, gl_TexCoord[0].st) * texture2D(depth, gl_TexCoord[1].st);\n"
    "}\n"
    ;
    
    GLShader fshader;
    GLProgram program;
    
    virtual void gl_run(GLTextureData<RGBAFrame>&, GLTextureData<DepthFrame>&) override final
    {
        GLWindowBinding *window = GLThread::getGLContextBinding<GLWindowBinding>();
        
        glViewport(0, 0, 600, 600);
        
        gl_Call(glMatrixMode(GL_MODELVIEW));
        gl_Call(glLoadIdentity());
        gl_Call(glMatrixMode(GL_PROJECTION));
        gl_Call(glLoadIdentity());
        gl_Call(glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0));
        
        gl_Call(glClearColor(0.0, 0.0, 0.0, 0.0));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        program.use();
        GLUniform(program, "color").set(0);
        GLUniform(program, "depth").set(1);
        
        gl_Call(glColor4d(0.0, 1.0, 0.0, 1.0));
        glBegin(GL_QUADS);
            glMultiTexCoord2d(GL_TEXTURE0, 0.0, 0.0);
            glMultiTexCoord2d(GL_TEXTURE1, 0.0, 0.0);
            glVertex2d(0.0, 0.0);
            
            glMultiTexCoord2d(GL_TEXTURE0, 1.0, 0.0);
            glMultiTexCoord2d(GL_TEXTURE1, 1.0, 0.0);
            glVertex2d(1.0, 0.0);
            
            glMultiTexCoord2d(GL_TEXTURE0, 1.0, 1.0);
            glMultiTexCoord2d(GL_TEXTURE1, 1.0, 1.0);
            glVertex2d(1.0, 1.0);
            
            glMultiTexCoord2d(GL_TEXTURE0, 0.0, 1.0);
            glMultiTexCoord2d(GL_TEXTURE1, 0.0, 1.0);
            glVertex2d(0.0, 1.0);
        gl_Call(glEnd());
        window->swapBuffers();
    }
};

posixpp::PThreadBarrier barrier(3);

TextureFiller *filler;
FrameDisplayer *displayer;

int main()
{
    glstreamer::init();
    
    GLThread t1([]{
        TextureFiller filler;
        ::filler = &filler;
        barrier.wait();
        
        barrier.wait();
        
        for(int i = 0; i < 30; ++i)
            filler.execute();
        
        barrier.wait();
        
        barrier.wait();
    }, (GLContextBinding*)nullptr, ":0");
    
    GLThread t2([]{
        FrameDisplayer displayer;
        ::displayer = &displayer;
        barrier.wait();
        
        barrier.wait();
        
        for(int i = 0; i < 30; ++i)
            displayer.execute();
        
        barrier.wait();
        
        barrier.wait();
    }, (GLWindowBinding*)nullptr, ":0", 600, 600);
    
    barrier.wait();
    
    {
        glstreamer::ThreadedLink link0(filler->outputArg(0), displayer->inputArg(0), 3);
        glstreamer::ThreadedLink link1(filler->outputArg(1), displayer->inputArg(1), 3);
        barrier.wait();
        
        barrier.wait();
    }
    barrier.wait();
    
    t1.join();
    t2.join();
}
