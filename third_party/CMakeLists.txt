add_subdirectory(glfw)
add_library(glfw::glfw ALIAS glfw)

set(cmake_BUILD_STATIC)
add_subdirectory(glew-cmake)
add_library(glew::glew ALIAS libglew_static)

add_subdirectory(glm)