cmake_minimum_required(VERSION 3.10)
project(glad)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE glad-sources src/*.c include/*.h)

add_library(glad STATIC ${glad-sources})
target_include_directories(glad PUBLIC include)