#include <cstdio>

#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <FreeImagePlus.h>

#include <posixpp/PThreadBarrier.h>

#include "../glstreamer.h"
#include "../internalsinglelink.h"
#include "../threadedlink.h"

#include "../gl/gl.inc.h"

#include "../gl/GLGenericRenderer.h"
#include "../gl/GLObject.h"
#include "../gl/GLScreenSpliter.h"
#include "../gl/GLThread.h"
#include "../gl/GLWindowBinding.h"

#include "../gl/draw_funcs.h"
#include "../gl/parameter_types.h"

#include "../core/ConstProcessor.h"

class ShapeRenderer : public glstreamer_gl::GLGenericRenderer
{
public:
    ShapeRenderer (glstreamer_gl::ProjectionStyle projection = glstreamer_gl::ProjectionStyle::Frustum ):
    glstreamer_gl::GLGenericRenderer(0, 0, 0, 0, projection),
    importer(),
    scene(importer.ReadFile("head.obj", aiProcess_Triangulate | aiProcess_SortByPType)),
    meshes(scene->mNumMeshes),
    texAlbedo()
    {
        for(unsigned i = 0; i < scene->mNumMeshes; ++i)
        {
            meshes[i].mesh = scene->mMeshes[i];
            glstreamer_gl::transformIndex(meshes[i].mesh, meshes[i].indices);
        }
        
        fipImage albedo;
        albedo.load("texture/AdrianAlbedo.tga");
        albedo.convertTo32Bits();
        gl_Call(glBindTexture(GL_TEXTURE_2D, texAlbedo));
        gl_Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, albedo.getWidth(), albedo.getHeight(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, albedo.accessPixels()));
        gl_Call(glGenerateMipmap(GL_TEXTURE_2D));
    }
    
    virtual ~ShapeRenderer() noexcept {}
    
private:
    Assimp::Importer importer;
    aiScene const* scene;
    struct Mesh
    {
        Mesh(): mesh(), indices() {}
        Mesh(Mesh const&) = default;
        Mesh& operator = (Mesh const&) = default;
        
        aiMesh const* mesh;
        std::vector<unsigned> indices;
    };
    std::vector<Mesh> meshes;
    
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
        
        gl_Call(glEnable(GL_TEXTURE_2D));
        for(Mesh const& mesh : meshes)
            glstreamer_gl::drawMeshWithIndex(mesh.mesh, mesh.indices);
        
#if 0
        gl_Call(glLoadIdentity());
        gl_Call(glTranslated(0.0, 0.0, -2.0));
        gl_Call(glRotated(10, 1.0, 0.0, 0.0));
        gl_Call(glRotated(30, 0.0, 1.0, 0.0));
        
        gl_Call(glColor4d(0.5, 0.5, 0.0, 1.0));
        glBegin(GL_QUADS);
            glVertex2d(-1.0, -1.0);
            glVertex2d( 1.0, -1.0);
            glVertex2d( 1.0,  1.0);
            glVertex2d(-1.0,  1.0);
        gl_Call(glEnd());
        
        gl_Call(glLoadIdentity());
        gl_Call(glTranslated(0.0, 0.0, -2.0));
        gl_Call(glRotated(10, 1.0, 0.0, 0.0));
        gl_Call(glRotated(-30, 0.0, 1.0, 0.0));
        
        gl_Call(glColor4d(0.5, 0.0, 0.5, 1.0));
        glBegin(GL_QUADS);
            glVertex2d(-1.0, -1.0);
            glVertex2d( 1.0, -1.0);
            glVertex2d( 1.0,  1.0);
            glVertex2d(-1.0,  1.0);
        gl_Call(glEnd());
#endif
    }
};

posixpp::PThreadBarrier barrier(7);
ShapeRenderer* renderers[6];
std::unique_ptr<glstreamer_gl::GLThread> threads[6];

static constexpr int loops = 60 * 10;

int main()
{
    using namespace glstreamer;
    using namespace glstreamer_core;
    using namespace glstreamer_gl;
    
    init();
    
    ConstProcessor<GLViewport> viewport(GLViewport({600, 600, -1.0, 1.0, -1.0, 1.0, 1.0, 5.0}));
    GLScreenSplitter spliter(GLScreenSplitter::makeGrid(3, 2));
    for(int y = 0; y < 2; ++y)
    {
        for(int x = 0; x < 3; ++x)
        {
            int i = y * 3 + x;
            threads[i] = std::unique_ptr<GLThread>(new GLThread([i]{
                std::unique_ptr<ShapeRenderer> renderer(new ShapeRenderer);
                renderers[i] = renderer.get();
                barrier.wait();
                barrier.wait();
                for(int i = 0; i < loops; ++i)
                {
                    renderer->execute();
                }
                barrier.wait();
                barrier.wait();
            }, (GLWindowBinding*)nullptr, ":0", 200, 300, true, x * 200 + 32, (1-y) * 300 + 32));
        }
    }
    barrier.wait();
    {
        InternalSingleLink constLink(viewport.outputArg(0), spliter.inputArg(0));
        std::unique_ptr<ThreadedLink> links[6];
        for(int i = 0; i < 6; ++i)
            links[i] = std::unique_ptr<ThreadedLink>(new ThreadedLink(spliter.outputArg(i), renderers[i]->inputArg(0), 3));
        barrier.wait();
        for(int i = 0; i < loops; ++i)
        {
            viewport.execute();
            spliter.execute();
        }
        barrier.wait();
    }
    barrier.wait();
    for(int i = 0; i < 6; ++i)
        threads[i]->join();
}
