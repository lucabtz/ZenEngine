#include "ZenEngine/Core/Platform.h"

#include <misc/cpp/imgui_stdlib.cpp>


#ifdef ZE_RENDERER_PLATFORM_OPENGL
    #define IMGUI_IMPL_OPENGL_LOADER_GLAD
    #include <backends/imgui_impl_opengl3.cpp>
#else
    #error "Render platform not supported by ImGui!"
#endif

#ifdef ZE_WINDOW_PLATFORM_GLFW
    #include <backends/imgui_impl_glfw.cpp>
#else
    #error "Window platform not supported by ImGui!"
#endif