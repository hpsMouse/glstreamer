#include <posixpp/PThreadBarrier.h>

#include "../gl/ogl.inc.h"

#include "../glstreamer.h"
#include "../threadedlink.h"

#include "../gl/GLContextBinding.h"
#include "../gl/GLFixedProcessor.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_gl;
using namespace oglplus;

class TextureFiller : public GLFixedProcessor<TypeList<>, TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>>
{
public:
    TextureFiller():
    GLFixedProcessor<TypeList<>, TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>>("RedFrame", "BlueFrame"),
    fbo()
    {}
    
private:
    Framebuffer fbo;
    
    void gl_run(GLTextureData<RGBAFrame>& red, GLTextureData<RGBAFrame>& blue) override final
    {
        red.resize(600, 600);
        blue.resize(600, 600);
        
        fbo.Bind(FramebufferTarget::Draw);
        
        fbo.AttachTexture2D(FramebufferTarget::Draw, FramebufferColorAttachment::_0, red.target, red.obj(), 0);
        gl.ClearColor(1.0, 0.0, 0.0, 1.0);
        gl.Clear().ColorBuffer();
        
        fbo.AttachTexture2D(FramebufferTarget::Draw, FramebufferColorAttachment::_0, blue.target, blue.obj(), 0);
        gl.ClearColor(0.0, 0.0, 1.0, 1.0);
        gl.Clear().ColorBuffer();
        
        write(2, "Readback finished\n", sizeof("Readback finished\n") - 1);
    }
};

class FrameDisplayer : public GLFixedProcessor<TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>, TypeList<>>
{
public:
    FrameDisplayer():
    GLFixedProcessor<TypeList<>, TypeList<>, TypeList<RGBAFrame, RGBAFrame>, TypeList<>>("RedFrame", "BlueFrame"),
    fshader(),
    program()
    {
        fshader.Source(fshaderSource).Compile();
        program.AttachShader(fshader).Link();
    }
    
private:
    static constexpr char const* fshaderSource = 
    "#version 120\n"
    "uniform sampler2D red;\n"
    "uniform sampler2D blue;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(red, gl_TexCoord[0].st) + texture2D(blue, gl_TexCoord[1].st);\n"
    "}\n"
    ;
    
    FragmentShader fshader;
    Program program;
    
    virtual void gl_run(GLTextureData<RGBAFrame>&, GLTextureData<RGBAFrame>&) override final
    {
        GLWindowBinding *window = GLThread::getGLContextBinding<GLWindowBinding>();
        glc.MatrixMode(oglplus::CompatibilityMatrixMode::Modelview);
        glc.LoadIdentity();
        glc.MatrixMode(oglplus::CompatibilityMatrixMode::Projection);
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
        
        gl.ClearColor(0.0, 0.0, 0.0, 0.0);
        gl.Clear().ColorBuffer().DepthBuffer();
        
        program.Use();
        Uniform<int>(program, "red").Set(0);
        Uniform<int>(program, "blue").Set(1);
        
        glc.Color(0.0, 1.0, 0.0, 1.0);
        glc.Begin(oglplus::CompatibilityPrimitiveType::Quads);
        glMultiTexCoord2d(GL_TEXTURE0, 0.0, 0.0);
        glMultiTexCoord2d(GL_TEXTURE1, 0.0, 0.0);
        glc.Vertex(0.0, 0.0);
        
        glMultiTexCoord2d(GL_TEXTURE0, 1.0, 0.0);
        glMultiTexCoord2d(GL_TEXTURE1, 1.0, 0.0);
        glc.Vertex(1.0, 0.0);
        
        glMultiTexCoord2d(GL_TEXTURE0, 1.0, 1.0);
        glMultiTexCoord2d(GL_TEXTURE1, 1.0, 1.0);
        glc.Vertex(1.0, 1.0);
        
        glMultiTexCoord2d(GL_TEXTURE0, 1.0, 1.0);
        glMultiTexCoord2d(GL_TEXTURE1, 1.0, 1.0);
        glc.Vertex(0.0, 1.0);
        glc.End();
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
