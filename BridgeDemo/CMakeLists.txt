cmake_minimum_required(VERSION 3.10)
project(BridgePhysics)

# Setting the root to your specific demo folder
set(PROJECT_ROOT "G:/bullet_demo")
set(DEMO_DIR "${PROJECT_ROOT}/BridgeDemo")

# 1. Include Paths - Pointing to the folders INSIDE BridgeDemo
include_directories(
    .
    ${PROJECT_ROOT}/src
    ${DEMO_DIR}                          # So it finds BridgeDemo/Utils, etc.
    ${DEMO_DIR}/ThirdPartyLibs
    ${DEMO_DIR}/ThirdPartyLibs/glad
    ${DEMO_DIR}/ThirdPartyLibs/Wavefront
    ${DEMO_DIR}/ThirdPartyLibs/stb_image
)

# 2. Source Files - Grouping the utility files you copied
set(UTIL_SRC
    #${DEMO_DIR}/Importers/ImportMeshUtility/b3ImportMeshUtility.cpp # commented out to avoid the linker error.
    ${DEMO_DIR}/Importers/ImportObjDemo/Wavefront2GLInstanceGraphicsShape.cpp
    ${DEMO_DIR}/Utils/b3ResourcePath.cpp
    ${DEMO_DIR}/Utils/b3Clock.cpp
   
)
set(EXAMPLE_BROWSER_SRC
    ${DEMO_DIR}/ExampleBrowser/OpenGLGuiHelper.cpp
    ${DEMO_DIR}/ExampleBrowser/CollisionShape2TriangleMesh.cpp
)

set(OPENGL_SRC
    ${DEMO_DIR}/OpenGLWindow/Win32Window.cpp
    ${DEMO_DIR}/OpenGLWindow/Win32OpenGLWindow.cpp
    ${DEMO_DIR}/OpenGLWindow/SimpleOpenGL3App.cpp
    ${DEMO_DIR}/OpenGLWindow/SimpleCamera.cpp
    ${DEMO_DIR}/OpenGLWindow/fontstash.cpp
    ${DEMO_DIR}/OpenGLWindow/opengl_fontstashcallbacks.cpp
    ${DEMO_DIR}/OpenGLWindow/GLPrimitiveRenderer.cpp
    ${DEMO_DIR}/OpenGLWindow/GLInstancingRenderer.cpp
    ${DEMO_DIR}/OpenGLWindow/GLRenderToTexture.cpp
    ${DEMO_DIR}/OpenGLWindow/LoadShader.cpp
    ${DEMO_DIR}/OpenGLWindow/OpenSans.cpp
    ${DEMO_DIR}/OpenGLWindow/TwFonts.cpp
)

add_executable(BridgeApp 
    main.cpp 
    Bridge.cpp 
    linker_fixes.cpp
    #${DEMO_DIR}/Importers/ImportObjDemo/LoadMeshFromObj.cpp  #earlier used for the obj
    ${DEMO_DIR}/Importers/ImportSTLDemo/ImportSTLSetup.cpp
    ${DEMO_DIR}/ThirdPartyLibs/glad/gl.c
    ${UTIL_SRC}
    ${OPENGL_SRC}
    ${EXAMPLE_BROWSER_SRC}
)
# 3. Linking
# Using link_directories + library names is the most stable way for MinGW
link_directories("${PROJECT_ROOT}/build/lib")

target_link_libraries(BridgeApp 
PRIVATE 
    BulletDynamics 
    BulletCollision 
    LinearMath
    Bullet3Common  # Added: Required for Utils/B3Logging
    opengl32
    glu32
    gdi32
    winmm
)
# Essential definitions for standalone Windows build
target_compile_definitions(BridgeApp PRIVATE _WIN32 B3_USE_STANDALONE_EXAMPLE)