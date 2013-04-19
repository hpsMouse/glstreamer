#include <stdint.h>

#include <vector>

#include <unistd.h>

#include <GL/glew.h>

#include "../glstreamer.h"

#include "../gl/GLContextBinding.h"

int main()
{
    glstreamer::init();
    glstreamer::loadExternalPlugin("./libglstreamer_gl.so");
    
    glstreamer_gl::GLContextBinding context(":0");
    
    glewInit();
    
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    GLuint renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 800, 600);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
    
    glClearColor(0.75, 0.5, 0.25, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    std::vector<uint32_t> buffer(800*600);
    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data());
    
    write(1, buffer.data(), buffer.size() * sizeof(buffer[0]));
}
