#include <cstdio>

#include <memory>

#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../internalsinglelink.h"
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

posixpp::PThreadBarrier barrier(7);
GLObjectRenderer *renderers[6];
GLThread threads[6];

static constexpr int loops = 60;

int main()
{
    init();
    
    unsigned width = 600, height = 600;
    GLViewport canvasViewport({0, 0, width, height, -(double(width)/height), double(width)/height, -1.0, 1.0, 12.0, 64.0});
    GLDataRange fullRange({0.0, 1.0});
    GLObjectState initState({0.1, 3.0, 0.5, -24.0, 0.0, 190.0, 0.0, 10.0, 0.0, 0.0});
    
    GLWindowBinding context(":0", width, height, true, 32 + 600, 32);
    GLThread::initGLContextBinding(context);
    
    for(int y = 0; y < 2; ++y)
    {
        for(int x = 0; x < 3; ++x)
        {
            int i = y * 3 + x;
            threads[i] = GLThread([i]{
//                 GLObjectRenderer renderer("head.obj", "texture/AdrianAlbedo.tga");
                GLObjectRenderer renderer("mcreg/minecraft.obj", "mcreg/tex/texture.png");
                renderers[i] = &renderer;
                FakeSink<GLFrameData<DepthFrame>> depthSink;
                barrier.wait();
                
                InternalSingleLink depthLink(renderer.outputArg("depth_out", 0), depthSink.inputArg(0));
                barrier.wait();
                
                for(int i = 0; i < loops; ++i)
                {
                    renderer.execute();
                    depthSink.execute();
                }
                barrier.wait();
                
                barrier.wait();
            }, (GLWindowBinding*)nullptr, ":0", width / 3, height / 2, true, x * width / 3 + 32, (1-y) * height / 2 + 32);
        }
    }
    
    ConstProcessor<GLViewport> viewport(canvasViewport, 2);
    ConstProcessor<GLDataRange> range(fullRange, 6);
    VariableProcessor<GLObjectState> state(6);
    
    GLScreenSplitter splitter(GLScreenSplitter::makeGrid(3, 2));
    
    GLFrameComposer composer(6, GLFrameComposer::ColorOnly());
    GLFrameDisplayer displayer;
    FakeSink<GLFrameData<DepthFrame>> depthSink;
    FakeSink<GLViewport> viewportFakeSink;
    barrier.wait();
    {
        InternalSingleLink viewportLink(viewport.outputArg(0), splitter.inputArg(0));
        InternalSingleLink canvasViewportLink(viewport.outputArg(1), composer.inputArg("canvas_viewport_in"));
        std::unique_ptr<ThreadedLink> viewportLinks[6], rangeLinks[6], stateLinks[6], composerViewports[6], composerColors[6];
        InternalSingleLink colorLink(composer.outputArg("color_out"), displayer.inputArg(0));
        InternalSingleLink depthLink(composer.outputArg("depth_out"), depthSink.inputArg(0));
        InternalSingleLink viewportFakeLink(composer.outputArg("canvas_viewport_out"), viewportFakeSink.inputArg(0));
        for(int i = 0; i < 6; ++i)
        {
            viewportLinks[i].reset(new ThreadedLink(splitter.outputArg(i), renderers[i]->inputArg("viewport_in"), 3));
            rangeLinks[i].reset(new ThreadedLink(range.outputArg(i), renderers[i]->inputArg("range"), 3));
            stateLinks[i].reset(new ThreadedLink(state.outputArg(i), renderers[i]->inputArg("state"), 3));
            composerViewports[i].reset(new ThreadedLink(renderers[i]->outputArg("viewport_out"), composer.inputArg("frame_viewport", i), 3));
            composerColors[i].reset(new ThreadedLink(renderers[i]->outputArg("color_out", 0), composer.inputArg("color_in", i), 3));
        }
        barrier.wait();
        
        state.set(initState);
        double start_time = getTimeAsDouble();
        for(int i = 0; i < loops; ++i)
        {
            //double current_time = getTimeAsDouble();
            //state.get().ry = (current_time - start_time) * 30;
            viewport.execute();
            range.execute();
            state.execute();
            splitter.execute();
            composer.execute();
            displayer.execute();
            depthSink.execute();
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
    for(int i = 0; i < 6; ++i)
        threads[i].join();
}
