cmake_minimum_required(VERSION 3.10)
project(BridgePhysics)

set(BULLET_ROOT "G:/bullet3") 

# 1. Include Paths - We are adding the BASE ThirdPartyLibs folder
# This allows #include <glad/gl.h> to find G:/bullet3/examples/ThirdPartyLibs/glad/gl.h
include_directories(
    .
    ${BULLET_ROOT}/src
    ${BULLET_ROOT}/examples
    ${BULLET_ROOT}/examples/ThirdPartyLibs
    ${BULLET_ROOT}/examples/ThirdPartyLibs/glad
    ${BULLET_ROOT}/examples/ThirdPartyLibs/glad/include
)

# 2. Source Files
file(GLOB_RECURSE STB_SRC "${BULLET_ROOT}/examples/ThirdPartyLibs/stb_image/*.cpp")

add_executable(BridgeApp 
    main.cpp 
    Bridge.cpp 
    ${BULLET_ROOT}/examples/ThirdPartyLibs/glad/gl.c
    ${STB_SRC}
  
    # Windows OpenGL Support
    ${BULLET_ROOT}/examples/OpenGLWindow/Win32Window.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/Win32OpenGLWindow.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/SimpleOpenGL3App.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/SimpleCamera.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/fontstash.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/opengl_fontstashcallbacks.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/GLPrimitiveRenderer.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/GLInstancingRenderer.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/GLRenderToTexture.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/LoadShader.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/OpenSans.cpp
    ${BULLET_ROOT}/examples/OpenGLWindow/TwFonts.cpp
)

# 3. Linking - Using the py3 names that reached 94% previously
target_link_libraries(BridgeApp 
PRIVATE 
    Bullet3Common 
    BulletExampleBrowserLib
    ${BULLET_ROOT}/build/lib/libBulletDynamics.a
    ${BULLET_ROOT}/build/lib/libBulletCollision.a
    ${BULLET_ROOT}/build/lib/libLinearMath.a
    opengl32
    glu32
    gdi32
    winmm
)

target_compile_definitions(BridgeApp PRIVATE _WIN32)