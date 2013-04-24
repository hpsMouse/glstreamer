#include <cstdint>

#include <thread>
#include <vector>

#include <pthread.h>

#include <unistd.h>

#include <GL/glew.h>

#include "../gl/GLEWContextBinding.h"

#include <oglplus/all.hpp>

#include "../glstreamer.h"

#include "../gl/GLContextBinding.h"

pthread_barrier_t barrier1, barrier2;

void renderto(GLclampf r, GLclampf g, GLclampf b, GLclampf a, std::vector<std::uint32_t>& buffer)
{
    pthread_barrier_wait(&barrier1);
    
    glstreamer_gl::GLContextBinding context(":0");
    
    using glstreamer_gl::glewGetContext;
    
    glewInit();
    
    pthread_barrier_wait(&barrier2);
    
    using namespace oglplus;
    
    Framebuffer fbo;
    fbo.Bind(FramebufferTarget::Draw);
    
    Renderbuffer renderbuffer;
    renderbuffer.Bind();
    Renderbuffer::Storage(RenderbufferTarget::Renderbuffer, PixelDataInternalFormat::RGBA, 800, 600);
    
    Framebuffer::AttachColorRenderbuffer(FramebufferTarget::Draw, 0, renderbuffer);
    
    Context::ClearColor(r, g, b, a);
    Context::Clear().ColorBuffer();
    
    fbo.Bind(FramebufferTarget::Read);
    buffer.resize(800*600);
    Context::ReadPixels(0, 0, 800, 600, PixelDataFormat::RGBA, PixelDataType::UnsignedInt_8_8_8_8_Rev, buffer.data());
}

int main()
{
    glstreamer::init();
    glstreamer::loadExternalPlugin("./libglstreamer_gl.so");
    
    int err = pthread_barrier_init(&barrier1, nullptr, 2);
    if(err)
        return 1;
    
    err = pthread_barrier_init(&barrier2, nullptr, 2);
    if(err)
        return 2;
    
    std::vector<std::uint32_t> buffers[2];
    std::thread t1([&buffers]{renderto(0.75, 0.50, 0.25, 1.0, buffers[0]);}), t2([&buffers]{renderto(0.25, 0.50, 0.75, 1.0, buffers[1]);});
    t1.join();
    t2.join();
    
    for(int i = 0; i < 2; ++i)
    {
        std::vector<std::uint32_t> const& buffer = buffers[i];
        write(1, buffer.data(), buffer.size() * sizeof(buffer[0]));
    }
}
