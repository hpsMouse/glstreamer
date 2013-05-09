#include "GLObjectRenderer.h"

#include "loaders.h"
#include "parameter_types.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLObjectRenderer::GLObjectRenderer ( const char* objectPath, const char* texturePath, bool show, ProjectionStyle projection ):
GLFBORenderer(0, 0, true, true, show, projection),
meshes(),
texture(load2DTexture(texturePath))
{
    loadMeshes(objectPath, meshes);
    
    inputArgs.addSlot<GLObjectState>("state");
}

void GLObjectRenderer::draw()
{
    gl_Call(glEnable(GL_DEPTH_TEST));
    gl_Call(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    
    auto const& state = inputArg("state").toSlot().arg<GLObjectState>();
    
    gl_Call(glMatrixMode(GL_MODELVIEW));
    gl_Call(glLoadIdentity());
    state.apply();
    
    auto const& range = this->getDataRange();
    
    gl_Call(glActiveTexture(GL_TEXTURE0));
    gl_Call(glBindTexture(GL_TEXTURE_2D, texture));
    gl_Call(glEnable(GL_TEXTURE_2D));
    for(glstreamer_gl::GLMeshBuffer const& mesh : meshes)
        mesh.draw(range.start, range.end);
    gl_Call(glDisable(GL_TEXTURE_2D));
}
