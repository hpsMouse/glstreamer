#include <thread>

#include <pthread.h>

#include "../gl/ogl.inc.h"

#include "../glstreamer.h"

#include "../gl/GLWindowBinding.h"

pthread_barrier_t barrier1, barrier2;

void render(int x, int y, GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
    pthread_barrier_wait(&barrier1);
    
    glstreamer_gl::GLWindowBinding context(":0", 683, 768, true, x, y);
    
    context.getContext().printInfo();
    
    pthread_barrier_wait(&barrier2);
    
    for(int i = 0; i < 60; ++i)
    {
        using namespace oglplus;
        gl.ClearColor(r, g, b, a);
        gl.Clear().ColorBuffer().DepthBuffer();
        context.swapBuffers();
        gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.Clear().ColorBuffer().DepthBuffer();
        context.swapBuffers();
    }
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
    
    std::thread t1([]{render(0, 0, 1.0, 0.0, 0.0, 1.0);}), t2([]{render(683, 0, 0.0, 0.0, 1.0, 1.0);});
    t1.join();
    t2.join();
}
