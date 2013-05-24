#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../core/processors.h"
#include "../gl/processors.h"

#include "../internalsinglelink.h"
#include "../LinkBlock.h"
#include "../ThreadBlock.h"
#include "../threadedlink.h"

#include "../gl/GLContextBinding.h"
#include "../gl/GLShadowCoordCalc.h"
#include "../gl/GLSimpleShadowRenderer.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_core;
using namespace glstreamer_gl;

GLThread thread;
posixpp::PThreadBarrier barrier(2);

static constexpr int loops = 600;

char const* objPath = "village/minecraft.obj";
char const* texPath = "village/tex/texture.png";

int main()
{
    init();
    
    unsigned width = 1280, height = 720;
    unsigned lightSize = 1024;
    unsigned lightWidth = lightSize, lightHeight = lightSize;
    GLWindowBinding context(":0", width, height, true, 32, 32);
    GLThread::initGLContextBinding(context);
    
    double lightSpan = 50.0;
    double cameraSpan = 10.0;
    
//     GLObjectState state = {0.01, 0.0, 0.0, -3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    GLObjectState state = {1.0, 0.0, -5.0, -240.0, 0.0, 60.0, 0.0, 10.0, 0.0, 0.0};
    GLObjectState lightState = {1.0, 0.0, 0.0, -1600.0, 120.0, 30.0, 0.0, 0.0, 00.0, 0.0};
    GLViewport viewport = {0, 0, width, height, -cameraSpan * width / height, cameraSpan * width / height, -cameraSpan, cameraSpan, 120.0, 320.0};
    GLViewport lightViewport = {0, 0, lightWidth, lightHeight, -lightSpan, lightSpan, -lightSpan, lightSpan, 1550.0, 1650.0};
    
//     GLObjectState state = lightState;
//     GLViewport viewport = lightViewport;
    
    GLDataRange fullRange = {0.0, 1.0};
    
    thread = std::move(GLThread([]{
        ThreadBlock& p = GLThread::currentThreadBlock();p
        .addProcessor<GLShadowMapGenerator>("shadow_map", objPath, 1, std::vector<ProjectionStyle>({ProjectionStyle::Ortho}))
        .addProcessor<FakeSink<GLViewport>>("viewport_sink")
        ;
        
        barrier.wait();
        
        LinkBlock l;l
        .link<InternalSingleLink>(p["shadow_map"].outputArg("viewport_out", 0), p["viewport_sink"].inputArg(0))
        ;
        
        barrier.wait();
        
        for(int i = 0; i < loops; ++i)
        {
            p.execute();
        }
        
        barrier.wait();
        barrier.wait();
    }, (GLContextBinding*)nullptr, ":0"));
    
    ThreadBlock p;p
    .addProcessor<VariableProcessor<GLObjectState>>("state", 2)
    .addProcessor<ConstProcessor<GLViewport>>("viewport", lightViewport, 2)
    .addProcessor<VariableProcessor<GLObjectState>>("obj_state")
    .addProcessor<ConstProcessor<GLViewport>>("obj_viewport", viewport)
    .addProcessor<ConstProcessor<GLDataRange>>("range", fullRange, 2)
    .addProcessor<GLShadowCoordCalc>("matrix", ProjectionStyle::Ortho)
    .addProcessor<GLSimpleShadowRenderer>("shadower", 1, objPath, texPath)
    .addProcessor<GLFrameDisplayer>("display")
    .addProcessor<FakeSink<GLFrameData<DepthFrame>>>("depth_sink")
    .addProcessor<FakeSink<GLViewport>>("viewport_sink")
    ;
    
    barrier.wait();
    
    auto& shadowMap = thread.threadBlock()["shadow_map"];
    
    auto &stateP = dynamic_cast<VariableProcessor<GLObjectState>&>(p["state"]);
    auto &statePO = dynamic_cast<VariableProcessor<GLObjectState>&>(p["obj_state"]);
    stateP.set(lightState);
    statePO.set(state);
    
    {
        LinkBlock l;l
        .link<InternalSingleLink>(p["state"].outputArg(0), p["matrix"].inputArg("state"))
        .link<InternalSingleLink>(p["viewport"].outputArg(0), p["matrix"].inputArg("viewport"))
        .link<ThreadedLink>(p["state"].outputArg(1), shadowMap.inputArg("state", 0), 3)
        .link<ThreadedLink>(p["viewport"].outputArg(1), shadowMap.inputArg("viewport_in", 0), 3)
        .link<InternalSingleLink>(p["range"].outputArg(0), p["shadower"].inputArg("range"))
        .link<ThreadedLink>(p["range"].outputArg(1), shadowMap.inputArg("range", 0), 3)
        .link<InternalSingleLink>(p["matrix"].outputArg(0), p["shadower"].inputArg("shadow_matrix", 0))
        .link<ThreadedLink>(shadowMap.outputArg("depth", 0), p["shadower"].inputArg("depth_in", 0), 3)
        .link<InternalSingleLink>(p["obj_state"].outputArg(0), p["shadower"].inputArg("state"))
        .link<InternalSingleLink>(p["obj_viewport"].outputArg(0), p["shadower"].inputArg("viewport_in"))
        .link<InternalSingleLink>(p["shadower"].outputArg("color_out", 0), p["display"].inputArg(0))
        .link<InternalSingleLink>(p["shadower"].outputArg("depth_out", 0), p["depth_sink"].inputArg(0))
        .link<InternalSingleLink>(p["shadower"].outputArg("viewport_out"), p["viewport_sink"].inputArg(0))
        ;
        
        barrier.wait();
        
        for(int i = 0; i < 600; ++i)
        {
//             statePO.get().ry -= 1.0;
            p.execute();
        }
        
        barrier.wait();
    }
    barrier.wait();
    thread.join();
}
