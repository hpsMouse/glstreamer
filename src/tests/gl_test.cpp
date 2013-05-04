#include <thread>

#include <pthread.h>

#include "../gl/gl.inc.h"

#include "../glstreamer.h"

#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

pthread_barrier_t barrier1, barrier2;

void render(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
    pthread_barrier_wait(&barrier1);
    
    using namespace glstreamer_gl;
    auto context = GLThread::getGLContextBinding<GLWindowBinding>();
    context->getContext().printInfo();
    
    pthread_barrier_wait(&barrier2);
    
    for(int i = 0; i < 30; ++i)
    {
        gl_Call(glClearColor(r, g, b, a));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        context->swapBuffers();
        gl_Call(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        context->swapBuffers();
    }
}

int main()
{
    glstreamer::init();
    
    int err = pthread_barrier_init(&barrier1, nullptr, 2);
    if(err)
        return 1;
    
    err = pthread_barrier_init(&barrier2, nullptr, 2);
    if(err)
        return 2;
    
    using namespace glstreamer_gl;
    GLThread
    t1([]{render(1.0, 0.0, 0.0, 1.0);}, (GLWindowBinding*)nullptr, ":0", 683, 768, true, 0, 0),
    t2([]{render(0.0, 0.0, 1.0, 1.0);}, (GLWindowBinding*)nullptr, ":0", 683, 768, true, 683, 0);
    t1.join();
    t2.join();
}
