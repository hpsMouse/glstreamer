#include <memory>

#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../internalsinglelink.h"
#include "../threadedlink.h"

#include "../gl/gl.inc.h"

#include "../gl/GLDataRangeSplitter.h"
#include "../gl/GLFBORenderer.h"
#include "../gl/GLFrameDisplayer.h"
#include "../gl/GLObject.h"
#include "../gl/GLScreenSpliter.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

#include "../gl/loaders.h"
#include "../gl/parameter_types.h"

#include "../core/ConstProcessor.h"
#include "../core/FakeSink.h"

class ShapeRenderer : public glstreamer_gl::GLFBORenderer
{
public:
    ShapeRenderer (glstreamer_gl::ProjectionStyle projection = glstreamer_gl::ProjectionStyle::Frustum ):
    glstreamer_gl::GLFBORenderer(0, 0, true, true, projection),
    meshes(),
    texAlbedo(glstreamer_gl::load2DTexture("texture/AdrianAlbedo.tga"))
    {
        glstreamer_gl::loadMeshes("head.obj", meshes);
    }
    
    virtual ~ShapeRenderer() noexcept {}
    
private:
    std::vector<glstreamer_gl::GLMeshBuffer> meshes;
    
    glstreamer_gl::TextureObject texAlbedo;
    
    ShapeRenderer(ShapeRenderer const&) = delete;
    ShapeRenderer& operator = (ShapeRenderer const&) = delete;
    
    virtual void draw() override
    {
        gl_Call(glEnable(GL_DEPTH_TEST));
        gl_Call(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        gl_Call(glMatrixMode(GL_MODELVIEW));
        
        gl_Call(glLoadIdentity());
        gl_Call(glTranslated(0.0, 0.0, -3.0));
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

int main()
{
    using namespace glstreamer;
    using namespace glstreamer_core;
    using namespace glstreamer_gl;
    
    init();
    
    //ConstProcessor<GLViewport> viewport(GLViewport({600, 600, -1.0, 1.0, -1.0, 1.0, 1.0, 5.0}));
    ConstProcessor<GLViewport> viewport(GLViewport({300, 300, -1.0, 1.0, -1.0, 1.0, 1.0, 5.0}));
    ConstProcessor<GLDataRange> dataRange(GLDataRange({0.0, 1.0}));
    //GLScreenSplitter spliter(GLScreenSplitter::makeGrid(3, 2));
    GLScreenSplitter screenSplitter(std::vector<ScreenPart>(6, ScreenPart({0.0, 1.0, 0.0, 1.0})));
    GLDataRangeSplitter rangeSplitter(GLDataRangeSplitter::makeEvenSplit(6));
    //GLDataRangeSplitter rangeSplitter(std::vector<GLDataRange>(6, GLDataRange({0.0, 1.0})));
    for(int y = 0; y < 2; ++y)
    {
        for(int x = 0; x < 3; ++x)
        {
            int i = y * 3 + x;
            // No need to move, but KDevelop doesn't understand.
            threads[i] = std::move(GLThread([i]{
                std::unique_ptr<ShapeRenderer> renderer(new ShapeRenderer);
                renderers[i] = renderer.get();
                GLFrameDisplayer displayer;
                glstreamer_core::FakeSink<GLFrameData<DepthFrame>> depthSink;
                InternalSingleLink
                displayLink(renderer->outputArg(0), displayer.inputArg(0)),
                depthLink(renderer->outputArg(1), depthSink.inputArg(0));
                barrier.wait();
                barrier.wait();
                for(int i = 0; i < loops; ++i)
                {
                    renderer->execute();
                    displayer.execute();
                }
                barrier.wait();
                barrier.wait();
            }, (GLWindowBinding*)nullptr, ":0", 300, 300, true, x * 300 + 32, (1-y) * 300 + 32));
        }
    }
    barrier.wait();
    {
        InternalSingleLink screenConstLink(viewport.outputArg(0), screenSplitter.inputArg(0));
        InternalSingleLink rangeConstLink(dataRange.outputArg(0), rangeSplitter.inputArg(0));
        std::unique_ptr<ThreadedLink> screenLinks[6], rangeLinks[6];
        for(int i = 0; i < 6; ++i)
        {
            screenLinks[i] = std::unique_ptr<ThreadedLink>(new ThreadedLink(screenSplitter.outputArg(i), renderers[i]->inputArg(0), 3));
            rangeLinks[i] = std::unique_ptr<ThreadedLink>(new ThreadedLink(rangeSplitter.outputArg(i), renderers[i]->inputArg(1), 3));
        }
        barrier.wait();
        for(int i = 0; i < loops; ++i)
        {
            viewport.execute();
            dataRange.execute();
            screenSplitter.execute();
            rangeSplitter.execute();
        }
        barrier.wait();
    }
    barrier.wait();
    for(int i = 0; i < 6; ++i)
        threads[i].join();
}
