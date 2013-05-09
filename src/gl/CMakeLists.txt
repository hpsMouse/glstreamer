add_library(glstreamer_gl SHARED GLContext.cpp GLContextBinding.cpp GLDataRangeSplitter.cpp GLDepthDisplayer.cpp GLDisplay.cpp GLException.cpp GLFBConfig.cpp GLFBORenderer.cpp GLFrameComposer.cpp GLFrameDisplayer.cpp GLFrameTypeSpec.cpp GLGenericRenderer.cpp GLObjectRenderer.cpp GLProgram.cpp GLScreenSpliter.cpp GLShader.cpp GLShadowMapGenerator.cpp GLThread.cpp GLUniform.cpp GLWindow.cpp GLWindowBinding.cpp GLXException.cpp draw_funcs.cpp loaders.cpp plugin_gl.cpp)

target_link_libraries(glstreamer_gl assimp freeimage freeimageplus)
