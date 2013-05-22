#include <cstdio>

#include <memory>

#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../internalsinglelink.h"
#include "../threadedlink.h"

#include "../core/ConstProcessor.h"
#include "../core/FakeSink.h"
#include "../core/VariableProcessor.h"

#include "../gl/GLDataRangeSplitter.h"
#include "../gl/GLDepthDisplayer.h"
#include "../gl/GLFrameComposer.h"
#include "../gl/GLFrameDisplayer.h"
#include "../gl/GLObjectRenderer.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

#include "../gl/parameter_types.h"

#include "rdtsc.h"

using namespace glstreamer;
using namespace glstreamer_core;
using namespace glstreamer_gl;

posixpp::PThreadBarrier barrier(3);
GLObjectRenderer *renderers[2];
GLThread threads[2];

static constexpr int loops = 120;

int main()
{
    init();
    
    unsigned width = 400, height = 400;
    GLViewport canvasViewport({0, 0, width, height, -(double(width)/height), double(width)/height, -1.0, 1.0, 1.0, 5.0});
    GLDataRange fullRange({0.0, 1.0});
    GLObjectState initState({0.01, 0.0, 0.0, -3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    
    GLWindowBinding context(":0", width, height, true, 32 + width * 2, 32);
    GLThread::initGLContextBinding(context);
    
    for(int i = 0; i < 2; ++i)
    {
        threads[i] = std::move(GLThread([i]{
            GLObjectRenderer renderer("head.obj", "texture/AdrianAlbedo.tga");
            renderers[i] = &renderer;
            barrier.wait();
            
            barrier.wait();
            
            for(int j = 0; j < loops; ++j)
            {
                renderer.execute();
            }
            barrier.wait();
            
            barrier.wait();
        }, (GLWindowBinding*)nullptr, ":0", width, height, true, 32 + i * width, 32));
    }
    
    ConstProcessor<GLViewport> viewport(canvasViewport, 3);
    ConstProcessor<GLDataRange> range(fullRange, 1);
    VariableProcessor<GLObjectState> state(2);
    
    GLDataRangeSplitter splitter(GLDataRangeSplitter::makeEvenSplit(2));
    
    GLFrameComposer composer(2);
    GLFrameDisplayer displayer;
    FakeSink<GLFrameData<DepthFrame>> depthDisplayer;
//     FakeSink<GLFrameData<RGBAFrame>> displayer;
//     GLDepthDisplayer depthDisplayer;
    FakeSink<GLViewport> viewportFakeSink;
    
    barrier.wait();
    {
        InternalSingleLink canvasViewportLink(viewport.outputArg(2), composer.inputArg("canvas_viewport_in"));
        InternalSingleLink rangeLink(range.outputArg(0), splitter.inputArg(0));
        InternalSingleLink displayLink(composer.outputArg("color_out"), displayer.inputArg(0));
        InternalSingleLink depthLink(composer.outputArg("depth_out"), depthDisplayer.inputArg(0));
        InternalSingleLink viewportFakeLink(composer.outputArg("canvas_viewport_out"), viewportFakeSink.inputArg(0));
        
        std::unique_ptr<ThreadedLink> viewportLinks[2], rangeLinks[2], stateLinks[2], composerViewports[2], composerColors[2], composerDepths[2];
        for(int i = 0; i < 2; ++i)
        {
            viewportLinks[i].reset(new ThreadedLink(viewport.outputArg(i), renderers[i]->inputArg("viewport_in"), 3));
            rangeLinks[i].reset(new ThreadedLink(splitter.outputArg(i), renderers[i]->inputArg("range"), 3));
            stateLinks[i].reset(new ThreadedLink(state.outputArg(i), renderers[i]->inputArg("state"), 3));
            
            composerViewports[i].reset(new ThreadedLink(renderers[i]->outputArg("viewport_out"), composer.inputArg("frame_viewport", i), 3));
            composerColors[i].reset(new ThreadedLink(renderers[i]->outputArg("color_out", 0), composer.inputArg("color_in", i), 3));
            composerDepths[i].reset(new ThreadedLink(renderers[i]->outputArg("depth_out", 0), composer.inputArg("depth_in", i), 3));
        }
        barrier.wait();
        
        state.set(initState);
        double start_time = getTimeAsDouble();
        for(int i = 0; i < loops; ++i)
        {
            double current_time = getTimeAsDouble();
            state.get().ry = (current_time - start_time) * 30;
            viewport.execute();
            range.execute();
            state.execute();
            splitter.execute();
            composer.execute();
            displayer.execute();
            depthDisplayer.execute();
            viewportFakeSink.execute();
        }
        double end_time = getTimeAsDouble();
        std::printf("%d frames in %f seconds. %f Frames per second.\n",
                    loops,
                    end_time - start_time,
                    loops / (end_time - start_time)
        );
        
        barrier.wait();
    }
    barrier.wait();
    for(int i = 0; i < 2; ++i)
        threads[i].join();
}
