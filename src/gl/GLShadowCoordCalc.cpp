#include "GLShadowCoordCalc.h"

#include "gl.inc.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLShadowCoordCalc::GLShadowCoordCalc( ProjectionStyle projection ):
FixedProcessor<TypeList<GLObjectState, GLViewport>, TypeList<GLMatrix>>("state", "viewport", "matrix"),
projection(projection)
{}

void GLShadowCoordCalc::do_run ( GLObjectState& state, GLViewport& viewport, GLMatrix& matrix )
{
    gl_Call(glMatrixMode(GL_MODELVIEW));
    gl_Call(glLoadIdentity());
    gl_Call(glTranslated(0.5, 0.5, 0.5));
    gl_Call(glScaled(0.5, 0.5, 0.5));
    applyViewport(viewport, projection);
    state.apply();
    gl_Call(glGetFloatv(GL_MODELVIEW_MATRIX, matrix.elements[0]));
}
