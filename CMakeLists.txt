cmake_minimum_required(VERSION 3.0)
project(ZenEngineEcosystem)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan OPTIONAL_COMPONENTS shaderc_combined REQUIRED)

message("Vulkan includes are at ${Vulkan_INCLUDE_DIRS}")

include_directories(ZenEngine/src 
    ZenEngine/include 
    SYSTEM ZenEngine/vendor/spdlog/include 
    SYSTEM ZenEngine/vendor/glfw/include 
    SYSTEM ZenEngine/vendor/glad/include 
    SYSTEM ${Vulkan_INCLUDE_DIRS}
    SYSTEM ZenEngine/vendor/imgui
)

set(CMAKE_CXX_FLAGS_DEBUGEDITOR "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUGEDITOR "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGEDITOR "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGEDITOR "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

set(CMAKE_CXX_FLAGS_EDITOR "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_EDITOR "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_EDITOR "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_EDITOR "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES};Editor;DebugEditor")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug;DebugEditor>:Debug>")

set(DEBUG_CONFIGURATIONS "Debug;DebugEditor")

add_subdirectory(ZenEngine)
add_subdirectory(Sandbox)