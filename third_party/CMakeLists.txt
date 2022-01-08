add_subdirectory(glfw)
add_library(GLFW::GLFW ALIAS glfw)

set(cmake_BUILD_STATIC)
add_subdirectory(glew-cmake)
add_library(GLEW::GLEW ALIAS libglew_static)