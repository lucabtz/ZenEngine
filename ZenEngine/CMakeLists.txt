cmake_minimum_required(VERSION 3.0)
project(ZenEngine)

file(GLOB_RECURSE engine-sources src/ZenEngine/*.cpp src/ZenEngine/*.h src/Platform/*.h src/Platform/*.cpp)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)

add_library(ZenEngine STATIC ${engine-sources})

target_compile_definitions(ZenEngine PUBLIC "$<$<CONFIG:Debug>:ZE_DEBUG>")
target_compile_definitions(ZenEngine PUBLIC "$<$<CONFIG:DebugEditor>:ZE_DEBUG>")
target_compile_definitions(ZenEngine PUBLIC "$<$<CONFIG:DebugEditor>:WITH_EDITOR>")
target_compile_definitions(ZenEngine PUBLIC "$<$<CONFIG:Editor>:WITH_EDITOR>")

add_library(ImGUI 
    vendor/imgui/imconfig.h 
    vendor/imgui/imgui.h 
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_internal.h
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imstb_rectpack.h
    vendor/imgui/imstb_textedit.h
    vendor/imgui/imstb_truetype.h
    vendor/imgui/imgui_demo.cpp
)

add_library(ImGuizmo
  vendor/ImGuizmo/ImGuizmo.h
  vendor/ImGuizmo/ImGuizmo.cpp
)

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(Vulkan_LIB_DIR "$ENV{VULKAN_SDK}/Lib")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(Vulkan_LIB_DIR "$ENV{VULKAN_SDK}/Lib")
  endif()
else()
    set(Vulkan_LIB_DIR "$ENV{VULKAN_SDK}/lib")
endif()

target_include_directories(ZenEngine PUBLIC src)

target_link_libraries(ZenEngine 
    spdlog 
    glfw 
    glad 
    ImGUI
    ImGuizmo
)
target_link_libraries(ZenEngine debug "${Vulkan_LIB_DIR}/shaderc_combinedd.lib" optimized "${Vulkan_LIB_DIR}/shaderc_combined.lib")
target_link_libraries(ZenEngine debug "${Vulkan_LIB_DIR}/spirv-cross-cppd.lib" optimized "${Vulkan_LIB_DIR}/spirv-cross-cpp.lib")
target_link_libraries(ZenEngine debug "${Vulkan_LIB_DIR}/spirv-cross-cored.lib" optimized "${Vulkan_LIB_DIR}/spirv-cross-core.lib")
target_link_libraries(ZenEngine debug "${Vulkan_LIB_DIR}/spirv-cross-hlsld.lib" optimized "${Vulkan_LIB_DIR}/spirv-cross-hlsl.lib")
target_link_libraries(ZenEngine debug "${Vulkan_LIB_DIR}/spirv-cross-glsld.lib" optimized "${Vulkan_LIB_DIR}/spirv-cross-glsl.lib")
target_link_libraries(ZenEngine debug "${Vulkan_LIB_DIR}/spirv-cross-reflectd.lib" optimized "${Vulkan_LIB_DIR}/spirv-cross-reflect.lib")

