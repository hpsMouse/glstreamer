#include <cstdint>

#include <thread>
#include <vector>

#include <pthread.h>

#include <unistd.h>

#include <GL/glew.h>

#include "../glstreamer.h"

#include "../gl/GLContextBinding.h"
#include "../gl/GLEWContextBinding.h"

pthread_barrier_t barrier1, barrier2;

void renderto(GLclampf r, GLclampf g, GLclampf b, GLclampf a, std::vector<std::uint32_t>& buffer)
{
    pthread_barrier_wait(&barrier1);
    
    glstreamer_gl::GLContextBinding context(":0");
    
    using glstreamer_gl::glewGetContext;
    
    glewInit();
    
    pthread_barrier_wait(&barrier2);
    
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    GLuint renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 800, 600);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
    
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    buffer.resize(800*600);
    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data());
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
