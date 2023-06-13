cmake_minimum_required(VERSION 3.0)
project(ZenEngine)

file(GLOB_RECURSE engine-sources src/ZenEngine/*.cpp src/ZenEngine/*.h src/Platform/*.h src/Platform/*.cpp)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)


add_library(ZenEngine STATIC ${engine-sources})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building in DEBUG mode")
    target_compile_definitions(ZenEngine PUBLIC ZE_DEBUG)
endif()

target_include_directories(ZenEngine PUBLIC src)
target_link_libraries(ZenEngine spdlog glfw glad Vulkan::shaderc_combined)