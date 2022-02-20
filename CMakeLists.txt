cmake_minimum_required(VERSION 3.20)

project(OpenGLRayTrace)

set (CMAKE_CXX_STANDARD 20)

include(utils.cmake)

add_subdirectory(src)
add_subdirectory(third_party)

include(assets.cmake)