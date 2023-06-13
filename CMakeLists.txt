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
)

add_subdirectory(ZenEngine)
add_subdirectory(Sandbox)