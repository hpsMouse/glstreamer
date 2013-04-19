#include "../glstreamer.h"

#include "../gl/GLWindowBinding.h"

int main()
{
    glstreamer::init();
    glstreamer::loadExternalPlugin("./libglstreamer_gl.so");
    
    glstreamer_gl::GLWindowBinding context(":0", 800, 600);
    
    context.getContext().printInfo();
    
    for(int i = 0; i < 60; ++i)
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        context.swapBuffers();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        context.swapBuffers();
    }
}
