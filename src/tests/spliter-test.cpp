#include <cstdio>

#include <chrono>
#include <memory>

#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../internalsinglelink.h"
#include "../threadedlink.h"

#include "../gl/gl.inc.h"

#include "../gl/GLDataRangeSplitter.h"
#include "../gl/GLFBORenderer.h"
#include "../gl/GLFrameComposer.h"
#include "../gl/GLFrameDisplayer.h"
#include "../gl/GLObject.h"
#include "../gl/GLScreenSpliter.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

#include "../gl/loaders.h"
#include "../gl/parameter_types.h"

#include "../core/ConstProcessor.h"
#include "../core/Dispatcher.h"
#include "../core/FakeSink.h"
#include "../core/VariableProcessor.h"

class ShapeRenderer : public glstreamer_gl::GLFBORenderer
{
public:
    ShapeRenderer (glstreamer_gl::ProjectionStyle projection = glstreamer_gl::ProjectionStyle::Frustum ):
    glstreamer_gl::GLFBORenderer(0, 0, true, true, true, projection),
    meshes(),
    texAlbedo(glstreamer_gl::load2DTexture("texture/AdrianAlbedo.tga"))
    {
        glstreamer_gl::loadMeshes("head.obj", meshes);
        
        inputArgs.addSlot<double>("rotation");
    }
    
    virtual ~ShapeRenderer() noexcept {}
    
private:
    std::vector<glstreamer_gl::GLMeshBuffer> meshes;
    
    glstreamer_gl::TextureObject texAlbedo;
    
    ShapeRenderer(ShapeRenderer const&) = delete;
    ShapeRenderer& operator = (ShapeRenderer const&) = delete;
    
    virtual void draw() override
    {
        double rotation = *static_cast<double*>(inputArg("rotation").toSlot().simpleSlot->arg);
        
        gl_Call(glEnable(GL_DEPTH_TEST));
        gl_Call(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        gl_Call(glMatrixMode(GL_MODELVIEW));
        
        gl_Call(glLoadIdentity());
        gl_Call(glTranslated(0.0, 0.0, -3.0));
        gl_Call(glRotated(rotation, 0.0, 1.0, 0.0));
        gl_Call(glScaled(0.01, 0.01, 0.01));
        
        auto const& range = this->getDataRange();
        
        gl_Call(glActiveTexture(GL_TEXTURE0));
        gl_Call(glBindTexture(GL_TEXTURE_2D, texAlbedo));
        gl_Call(glEnable(GL_TEXTURE_2D));
        for(glstreamer_gl::GLMeshBuffer const& mesh : meshes)
            mesh.draw(range.start, range.end);
        gl_Call(glDisable(GL_TEXTURE_2D));
    }
};

posixpp::PThreadBarrier barrier(7);
ShapeRenderer* renderers[6];
glstreamer_gl::GLThread threads[6];

static constexpr int loops = 60 * 10;

// WARNING: Do NOT define both SORT_FIRST and SORT_LAST!

//#define SORT_FIRST
#define SORT_LAST

int main()
{
    using namespace glstreamer;
    using namespace glstreamer_core;
    using namespace glstreamer_gl;
    
    init();
    
    unsigned width = 640, height = 480;
    GLViewport viewportGlobal({0, 0, width, height, -(double(width)/height), double(width)/height, -1.0, 1.0, 1.0, 5.0});
    GLDataRange rangeGlobal({0.0, 1.0});
    
#ifdef SORT_LAST
    GLWindowBinding context(":0", width, height, true, 32, 32);
    GLThread::initGLContextBinding(context);
    
    ConstProcessor<GLViewport> viewportSource(viewportGlobal, 6);
    
    ConstProcessor<GLDataRange> range(GLDataRange({0.0, 1.0}));
    GLDataRangeSplitter rangeSource(GLDataRangeSplitter::makeEvenSplit(6));
    
    InternalSingleLink rangeLink(range.outputArg(0), rangeSource.inputArg(0));
    
    GLFrameComposer composer(6, std::vector<GLFrameInfo>({
        GLFrameInfo({0, 0}), GLFrameInfo({1, 1}), GLFrameInfo({2, 2}), GLFrameInfo({3, 3}), GLFrameInfo({4, 4}), GLFrameInfo({5, 5})
    }), 6, 6);
    FakeSink<GLViewport> viewportSink;
    GLFrameDisplayer displayer;
    FakeSink<GLFrameData<DepthFrame>> depthSink;
    
    InternalSingleLink viewportLink(composer.outputArg(0), viewportSink.inputArg(0));
    InternalSingleLink displayLink(composer.outputArg(1), displayer.inputArg(0));
    InternalSingleLink depthLink(composer.outputArg(2), depthSink.inputArg(0));
    
    ConstProcessor<GLViewport> viewportFinal(viewportGlobal);
    InternalSingleLink composerViewportLink(viewportFinal.outputArg(0), composer.inputArg("canvas_viewport_in"));
    
    auto action = [&]{
        viewportSource.execute();
        range.execute();
        rangeSource.execute();
        viewportFinal.execute();
        composer.execute();
        displayer.execute();
        depthSink.execute();
    };
#endif
    
#ifdef SORT_FIRST
    ConstProcessor<GLViewport> viewport(viewportGlobal);
    GLScreenSplitter viewportSource(GLScreenSplitter::makeGrid(3, 2));
    
    InternalSingleLink viewportLink(viewport.outputArg(0), viewportSource.inputArg(0));
    
    ConstProcessor<GLDataRange> rangeSource(rangeGlobal, 6);
    
    auto action = [&]{
        viewport.execute();
        viewportSource.execute();
        rangeSource.execute();
    };
#endif
    for(int y = 0; y < 2; ++y)
    {
        for(int x = 0; x < 3; ++x)
        {
            int i = y * 3 + x;
            // No need to move, but KDevelop doesn't understand.
            threads[i] = std::move(GLThread([i]{
                std::unique_ptr<ShapeRenderer> renderer(new ShapeRenderer);
                renderers[i] = renderer.get();
                
            #ifdef SORT_FIRST
                FakeSink<GLFrameData<RGBAFrame>> colorSink;
                FakeSink<GLFrameData<DepthFrame>> depthSink;
                InternalSingleLink colorLink(renderer->outputArg(0), colorSink.inputArg(0));
                InternalSingleLink depthLink(renderer->outputArg(1), depthSink.inputArg(0));
            #endif
                
                barrier.wait();
                barrier.wait();
                for(int i = 0; i < loops; ++i)
                {
                    renderer->execute();
            #ifdef SORT_FIRST
                    colorSink.execute();
                    depthSink.execute();
            #endif
                }
                barrier.wait();
                barrier.wait();
            },
        #ifdef SORT_LAST
            (GLWindowBinding*)nullptr, ":0", width, height, true, x * width / 3 + 32, (1-y) * height / 2 + 32));
        #endif
        #ifdef SORT_FIRST
            (GLWindowBinding*)nullptr, ":0", width/3, height/2, true, x * width / 3 + 32, (1-y) * height / 2 + 32));
        #endif
        }
    }
    barrier.wait();
    {
        VariableProcessor<double> rotation;
        Dispatcher<double> rotationSource(6);
        InternalSingleLink rotationLink(rotation.outputArg(0), rotationSource.inputArg(0));
        
        std::unique_ptr<ThreadedLink> viewportLinks[6], rangeLinks[6], rotationLinks[6];
        for(int i = 0; i < 6; ++i)
        {
            viewportLinks[i].reset(new ThreadedLink(viewportSource.outputArg(i), renderers[i]->inputArg(0), 3));
            rangeLinks[i].reset(new ThreadedLink(rangeSource.outputArg(i), renderers[i]->inputArg(1), 3));
            rotationLinks[i].reset(new ThreadedLink(rotationSource.outputArg(i), renderers[i]->inputArg("rotation"), 3));
        }
    #ifdef SORT_LAST
        std::unique_ptr<ThreadedLink> colorLinks[6], depthLinks[6], viewportParamLinks[6];
        for(int i = 0; i < 6; ++i)
        {
            viewportParamLinks[i].reset(new ThreadedLink(renderers[i]->outputArg(0), composer.inputArg("frame_viewport", i), 3));
            colorLinks[i].reset(new ThreadedLink(renderers[i]->outputArg(1), composer.inputArg("color_in", i), 3));
            depthLinks[i].reset(new ThreadedLink(renderers[i]->outputArg(2), composer.inputArg("depth_in", i), 3));
        }
    #endif
        barrier.wait();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < loops; ++i)
        {
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration(current_time - start_time);
            rotation.set(duration.count() * 30);
            rotation.execute();
            rotationSource.execute();
            action();
        }
        barrier.wait();
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration(end_time - start_time);
        printf("%d Frames in %f secs. %f frames/sec.\n", loops, duration.count(), loops/duration.count());
    }
    barrier.wait();
    for(int i = 0; i < 6; ++i)
        threads[i].join();
}
