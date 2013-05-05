add_library(glstreamer_gl SHARED GLContext.cpp GLContextBinding.cpp GLDisplay.cpp GLException.cpp GLFBConfig.cpp GLFrameTypeSpec.cpp GLGenericRenderer.cpp GLProgram.cpp GLScreenSpliter.cpp GLShader.cpp GLThread.cpp GLUniform.cpp GLWindow.cpp GLWindowBinding.cpp GLXException.cpp draw_funcs.cpp plugin_gl.cpp)

target_link_libraries(glstreamer_gl assimp freeimage freeimageplus)
