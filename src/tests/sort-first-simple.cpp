#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../internalsinglelink.h"
#include "../LinkBlock.h"
#include "../ThreadBlock.h"
#include "../threadedlink.h"

#include "../core/ConstProcessor.h"
#include "../core/FakeSink.h"
#include "../core/VariableProcessor.h"

#include "../gl/GLFrameComposer.h"
#include "../gl/GLFrameDisplayer.h"
#include "../gl/GLObjectRenderer.h"
#include "../gl/GLScreenSpliter.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

#include "../gl/parameter_types.h"

#include "rdtsc.h"

using namespace glstreamer;
using namespace glstreamer_core;
using namespace glstreamer_gl;

posixpp::PThreadBarrier barrier(3);
GLThread threads[2];

static constexpr int loops = 60;

int main()
{
    init();
    
    unsigned width = 600, height = 600;
    
    GLViewport canvasViewport({0, 0, width, height, -(double(width)/height), double(width)/height, -1.0, 1.0, 1.0, 5.0});
    GLDataRange fullRange({0.0, 1.0});
    GLObjectState initState({0.01, 0.0, 0.0, -3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    
    GLWindowBinding context(":0", width, height, true, 32 + width, 32);
    GLThread::initGLContextBinding(context);
    
    for(int i = 0; i < 2; ++i)
    {
        threads[i] = GLThread([i]{
            ThreadBlock& p = GLThread::currentThreadBlock();p
            .addProcessor<GLObjectRenderer>("renderer", "head.obj", "texture/AdrianAlbedo.tga")
            .addProcessor<FakeSink<GLFrameData<DepthFrame>>>("depth_sink")
            ;
            
            barrier.wait();
            
            LinkBlock l;l
            .link<InternalSingleLink>(p["renderer"].outputArg("depth_out", 0), p["depth_sink"].inputArg(0))
            ;
            barrier.wait();
            
            for(int j = 0; j < loops; ++j)
                p.execute();
            
            barrier.wait();
            
            barrier.wait();
        }, (GLWindowBinding*)nullptr, ":0", width, height / 2, true, 32, (1-i) * height / 2 + 32);
    }
    
    ThreadBlock p;p
    .addProcessor<ConstProcessor<GLViewport>>("viewport", canvasViewport, 2)
    .addProcessor<ConstProcessor<GLDataRange>>("range", fullRange, 2)
    .addProcessor<VariableProcessor<GLObjectState>>("state", 2)
    .addProcessor<GLScreenSplitter>("splitter", GLScreenSplitter::makeGrid(1, 2))
    .addProcessor<GLFrameComposer>("composer", 2, GLFrameComposer::ColorOnly())
    .addProcessor<GLFrameDisplayer>("displayer")
    .addProcessor<FakeSink<GLFrameData<DepthFrame>>>("depth_display")
    .addProcessor<FakeSink<GLViewport>>("viewport_sink")
    ;
    
    barrier.wait();
    {
        LinkBlock l;l
        .link<InternalSingleLink>(p["viewport"].outputArg(0), p["splitter"].inputArg(0))
        .link<InternalSingleLink>(p["viewport"].outputArg(1), p["composer"].inputArg("canvas_viewport_in"))
        .link<InternalSingleLink>(p["composer"].outputArg("color_out"), p["displayer"].inputArg(0))
        .link<InternalSingleLink>(p["composer"].outputArg("depth_out"), p["depth_display"].inputArg(0))
        .link<InternalSingleLink>(p["composer"].outputArg("canvas_viewport_out"), p["viewport_sink"].inputArg(0))
        ;
        for(int i = 0; i < 2; ++i)
        {
            l
            .link<ThreadedLink>(p["splitter"].outputArg(i), threads[i].threadBlock()["renderer"].inputArg("viewport_in"), 3)
            .link<ThreadedLink>(p["range"].outputArg(i), threads[i].threadBlock()["renderer"].inputArg("range"), 3)
            .link<ThreadedLink>(p["state"].outputArg(i), threads[i].threadBlock()["renderer"].inputArg("state"), 3)
            .link<ThreadedLink>(threads[i].threadBlock()["renderer"].outputArg("color_out", 0), p["composer"].inputArg("color_in", i), 3)
            .link<ThreadedLink>(threads[i].threadBlock()["renderer"].outputArg("viewport_out"), p["composer"].inputArg("frame_viewport", i), 3)
            ;
        }
        barrier.wait();
        
        auto& state = static_cast<VariableProcessor<GLObjectState>&>(p["state"]);
        state.set(initState);
        double start_time = getTimeAsDouble();
        for(int i = 0; i < loops; ++i)
        {
            double current_time = getTimeAsDouble();
            state.get().ry = (current_time - start_time) * 30;
            p.execute();
        }
        barrier.wait();
        double end_time = getTimeAsDouble();
        std::printf("%d frames in %f seconds. %f Frames per second.\n",
                    loops,
                    end_time - start_time,
                    loops / (end_time - start_time)
        );
    }
    barrier.wait();
    
    for(int i = 0; i < 2; ++i)
        threads[i].join();
}
