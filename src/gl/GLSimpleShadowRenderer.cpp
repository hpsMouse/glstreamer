#include "GLSimpleShadowRenderer.h"

#include "GLUniform.h"

#include "loaders.h"
#include "parameter_types.h"

using namespace glstreamer_gl;

char const* vertexSource =
"#version 120\n"

"uniform mat4 lightMatrix[8];\n"

"varying vec4 lightCoord[8];\n"
"varying vec3 normal;\n"

"vec4 pdiv(vec4 v)\n"
"{\n"
"    return v / v.w;\n"
"}\n"

"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"

"    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"

"    lightCoord[0] = pdiv(lightMatrix[0] * gl_Vertex);\n"
"    lightCoord[1] = pdiv(lightMatrix[1] * gl_Vertex);\n"
"    lightCoord[2] = pdiv(lightMatrix[2] * gl_Vertex);\n"
"    lightCoord[3] = pdiv(lightMatrix[3] * gl_Vertex);\n"
"    lightCoord[4] = pdiv(lightMatrix[4] * gl_Vertex);\n"
"    lightCoord[5] = pdiv(lightMatrix[5] * gl_Vertex);\n"
"    lightCoord[6] = pdiv(lightMatrix[6] * gl_Vertex);\n"
"    lightCoord[7] = pdiv(lightMatrix[7] * gl_Vertex);\n"

"    normal = normalize(gl_NormalMatrix * gl_Normal);\n"
"}\n"
;

char const* fragmentSource =
"#version 120\n"

"uniform sampler2D shadows[8];\n"
"uniform int nLights;\n"
"uniform sampler2D objTex;\n"

"varying vec4 lightCoord[8];\n"
"varying vec3 normal;\n"

"float shadowTex2D(sampler2D tex, vec2 coord, float ref)\n"
"{\n"
"    float depth = texture2D(tex, coord).r;\n"
// "    return depth;\n"
"    if(depth >= ref - 0.02)\n"
"        return 1.0;\n"
"    else\n"
"        return 0.0;\n"
"}\n"

"void main()\n"
"{\n"
"    float lightSum = shadowTex2D(shadows[0], lightCoord[0].xy, lightCoord[0].z);\n"
"    vec4 objTexValue = texture2D(objTex, gl_TexCoord[0].st);\n"
"    lightSum = lightSum * 0.75 + 0.25;\n"
// "    gl_FragColor = vec4(vec3(lightSum), 1.0);\n"
"    gl_FragColor = vec4(lightSum * objTexValue.rgb, 1.0);\n"
"}\n"
;

GLSimpleShadowRenderer::GLSimpleShadowRenderer ( std::size_t nLights, const char* objPath, const char* texturePath, bool show ):
GLFBORenderer (0, nLights, true, true, show, ProjectionStyle::Frustum),
nLights(nLights),
meshes(),
texture(load2DTexture(texturePath)),
program(),
vShader(GL_VERTEX_SHADER),
fShader(GL_FRAGMENT_SHADER),
lightMatrix()
{
    loadMeshes(objPath, meshes);
    
    vShader.addSourceStr(vertexSource).compile();
    fShader.addSourceStr(fragmentSource).compile();
    program.attachShader(vShader).attachShader(fShader).link();
    std::vector<GLint> shadows;
    shadows.reserve(nLights);
    for(std::size_t i = 0; i < nLights; ++i)
        shadows.push_back(i);
    program.use();
    GLUniform(program, "shadows").setV(glUniform1iv, nLights, shadows.data());
//     GLUniform(program, "nLights").set(GLint(nLights));
    GLUniform(program, "objTex").set(GLint(nLights));
    lightMatrix = GLUniform(program, "lightMatrix");
    program.useNone();
    
    inputArgs._addSlot<GLObjectState>("state");
    for(std::size_t i = 0; i < nLights; ++i)
    {
        inputArgs._addSlot<GLMatrix>(genName("shadow_matrix", i));
    }
    inputArgs.refreshSimpleSlots();
}

void GLSimpleShadowRenderer::draw()
{
    gl_Call(glClearColor(0.0, 0.0, 0.0, 0.0));
    gl_Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    
    int objectTexture = nLights;
    gl_Call(glActiveTexture(GL_TEXTURE0 + objectTexture));
    gl_Call(glBindTexture(GL_TEXTURE_2D, texture));
    
    gl_Call(glMatrixMode(GL_MODELVIEW));
    gl_Call(glLoadIdentity());
    inputArg("state").toSlot().arg<GLObjectState>().apply();
    
    gl_Call(glEnable(GL_DEPTH_TEST));
    
    program.use();
    
    std::vector<GLMatrix> lightMatrices;
    lightMatrices.reserve(8);
    for(std::size_t i = 0; i < nLights; ++i)
    {
        GLMatrix const& matrix = getMatrix(i);
        lightMatrices.push_back(matrix);
    }
    lightMatrix.setV(glUniformMatrix4fv, lightMatrices.size(), GL_FALSE, lightMatrices.data()->elements[0]);
    
    auto const& range = getDataRange();
    
    for(GLMeshBuffer const& mesh : meshes)
        mesh.draw(range.start, range.end);
    program.useNone();
    
    gl_Call(glDisable(GL_DEPTH_TEST));
}

GLMatrix const& GLSimpleShadowRenderer::getMatrix ( std::ptrdiff_t index )
{
    return inputArg("shadow_matrix", index).toSlot().arg<GLMatrix>();
}
