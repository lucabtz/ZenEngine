cmake_minimum_required(VERSION 3.0)
project(ZenEngineSandbox)

file(GLOB_RECURSE sandbox-sources src/*.cpp src/*.h)

add_executable(ZenEngineSanbox ${sandbox-sources})
target_compile_options(ZenEngineSanbox PUBLIC -Wall)
target_include_directories(ZenEngineSanbox PUBLIC src ../ZenEngine/include ../ZenEngine/src ../ZenEngine/vendor/spdlog/include)
target_link_libraries(ZenEngineSanbox ZenEngine)