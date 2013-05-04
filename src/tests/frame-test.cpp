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

class TextureFiller : public GLFixedProcessor<TypeList<>, TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>>
{
public:
    TextureFiller():
    GLFixedProcessor<TypeList<>, TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>>("RedFrame", "BlueFrame"),
    fbo(),
    redScale(1.0),
    blueScale(0.0)
    {}
    
private:
    FramebufferObject fbo;
    
    GLdouble redScale, blueScale;
    
    void gl_run(GLTextureData<RGBAFrame>& red, GLTextureData<RGBAFrame>& blue) override final
    {
        red.resize(600, 600);
        blue.resize(600, 600);
        
        gl_Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
        
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, red.obj(), 0));
        gl_Call(glClearColor(1.0 * redScale, 0.0, 0.0, 1.0));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT));
        
        gl_Call(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blue.obj(), 0));
        gl_Call(glClearColor(0.0, 0.0, 1.0 * blueScale, 1.0));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT));
        
        std::swap(redScale, blueScale);
        
        write(2, "Readback finished\n", sizeof("Readback finished\n") - 1);
    }
};

class FrameDisplayer : public GLFixedProcessor<TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>, TypeList<>>
{
public:
    FrameDisplayer():
    GLFixedProcessor<TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>, TypeList<>>("RedFrame", "BlueFrame"),
    fshader(GL_FRAGMENT_SHADER),
    program()
    {
        fshader.addSourceStr(fshaderSource).compile();
        program.attachShader(fshader).link();
    }
    
private:
    static constexpr char const* fshaderSource = 
    "#version 120\n"
    "uniform sampler2D red;\n"
    "uniform sampler2D blue;\n"
    "void main()\n"
    "{\n"
//    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "    gl_FragColor = texture2D(red, gl_TexCoord[0].st) + texture2D(blue, gl_TexCoord[1].st);\n"
    "}\n"
    ;
    
    GLShader fshader;
    GLProgram program;
    
    virtual void gl_run(GLTextureData<RGBAFrame>&, GLTextureData<RGBAFrame>&) override final
    {
        GLWindowBinding *window = GLThread::getGLContextBinding<GLWindowBinding>();
        gl_Call(glMatrixMode(GL_MODELVIEW));
        gl_Call(glLoadIdentity());
        gl_Call(glMatrixMode(GL_PROJECTION));
        gl_Call(glLoadIdentity());
        gl_Call(glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0));
        
        gl_Call(glClearColor(0.0, 0.0, 0.0, 0.0));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        program.use();
        GLUniform(program, "red").set(0);
        GLUniform(program, "blue").set(1);
        
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
