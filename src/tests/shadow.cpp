#include "../glstreamer.h"
#include "../core/processors.h"
#include "../gl/processors.h"

#include "../internalsinglelink.h"
#include "../LinkBlock.h"
#include "../ThreadBlock.h"

#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

using namespace glstreamer;
using namespace glstreamer_core;
using namespace glstreamer_gl;

int main()
{
    init();
    
    unsigned width = 512, height = 512;
    GLWindowBinding context(":0", width, height, true, 32, 32);
    GLThread::initGLContextBinding(context);
    
    GLObjectState state = {0.01, 0.0, 0.0, -3.0, 0.0, 0.0, 0.0};
    GLViewport viewport = {0, 0, width, height, -(double(width)/height), double(width)/height, -1.0, 1.0, 1.0, 5.0};
    GLDataRange fullRange = {0.0, 1.0};
    
    ThreadBlock p;p
    .addProcessor<VariableProcessor<GLObjectState>>("state")
    .addProcessor<ConstProcessor<GLViewport>>("viewport", viewport)
    .addProcessor<ConstProcessor<GLDataRange>>("range", fullRange)
    .addProcessor<GLShadowMapGenerator>("shadow", "head.obj", 1, std::vector<ProjectionStyle>({ProjectionStyle::Frustum}))
    .addProcessor<GLDepthDisplayer>("display")
    .addProcessor<FakeSink<GLViewport>>("viewport_sink")
    ;
    
    auto &stateP = dynamic_cast<VariableProcessor<GLObjectState>&>(p["state"]);
    stateP.set(state);
    
    LinkBlock l;l
    .link<InternalSingleLink>(p["state"].outputArg(0), p["shadow"].inputArg("state", 0))
    .link<InternalSingleLink>(p["viewport"].outputArg(0), p["shadow"].inputArg("viewport_in", 0))
    .link<InternalSingleLink>(p["range"].outputArg(0), p["shadow"].inputArg("range", 0))
    .link<InternalSingleLink>(p["shadow"].outputArg("depth", 0), p["display"].inputArg(0))
    .link<InternalSingleLink>(p["shadow"].outputArg("viewport_out", 0), p["viewport_sink"].inputArg(0))
    ;
    
    for(int i = 0; i < 600; ++i)
    {
        stateP.get().ry += 1.0;
        p.execute();
    }
}
