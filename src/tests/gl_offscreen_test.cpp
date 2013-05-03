#include <cstdint>

#include <thread>
#include <vector>

#include <pthread.h>

#include <unistd.h>

#include "../gl/ogl.inc.h"

#include "../glstreamer.h"
#include "../LocalArg.h"
#include "../typemgr.h"

#include "../gl/GLContextBinding.h"
#include "../gl/GLFrame.h"
#include "../gl/GLFrameTypeSpec.h"

pthread_barrier_t barrier1, barrier2;

using namespace glstreamer_gl;

typedef GLFrameData<RGBAFrame> RGBABuffer;
typedef GLTextureData<RGBAFrame> RGBATexture;
typedef GLFrameTypeSpec<RGBAFrame> RGBABufferSpec;

void renderto(GLclampf r, GLclampf g, GLclampf b, GLclampf a, RGBABuffer& buffer)
{
    pthread_barrier_wait(&barrier1);
    
    glstreamer_gl::GLContextBinding context(":0");
    
    glewInit();
    
    pthread_barrier_wait(&barrier2);
    
    using namespace oglplus;
    
    Framebuffer fbo;
    fbo.Bind(FramebufferTarget::Draw);
    
    RGBABufferSpec &spec = dynamic_cast<RGBABufferSpec&>(*glstreamer::TypeManager::getTypeSpec<RGBABuffer>());
    std::shared_ptr<glstreamer::LocalArgBase> localArg(spec.createLocal());
    RGBATexture &texture = localArg->getArg<RGBATexture>();
    texture.resize(800, 600);
    
    Framebuffer::AttachTexture2D(FramebufferTarget::Draw,
                                 Framebuffer::Property::Attachment(FramebufferColorAttachment::_0),
                                 texture.target,
                                 texture.obj(),
                                 0
                                );
    
    gl.ClearColor(r, g, b, a);
    gl.Clear().ColorBuffer();
    
    fbo.Bind(FramebufferTarget::Read);
    buffer.resize(800, 600);
    spec.context_out(&buffer, localArg.get());
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
    
    RGBABuffer buffers[2];
    std::thread t1([&buffers]{renderto(0.75, 0.50, 0.25, 1.0, buffers[0]);}), t2([&buffers]{renderto(0.25, 0.50, 0.75, 1.0, buffers[1]);});
    t1.join();
    t2.join();
    
    for(int i = 0; i < 2; ++i)
    {
        RGBABuffer const& buffer = buffers[i];
        write(1, buffer.data(), buffer.width() * buffer.height() * sizeof(RGBABuffer::Pixel));
    }
}
