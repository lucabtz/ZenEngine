cmake_minimum_required(VERSION 3.10)
project(ZenEngineSandbox)

file(GLOB_RECURSE sandbox-sources src/*.cpp src/*.h)

add_custom_target(CopyResources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ZenEngine/resources ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ZenEngine/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_executable(ZenEngineSandbox ${sandbox-sources})
add_dependencies(ZenEngineSandbox CopyResources)
target_compile_options(ZenEngineSandbox PUBLIC -Wall)
target_include_directories(ZenEngineSandbox PUBLIC src ../ZenEngine/include ../ZenEngine/src ../ZenEngine/vendor/spdlog/include)
target_link_libraries(ZenEngineSandbox ZenEngine)