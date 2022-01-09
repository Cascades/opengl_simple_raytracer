cmake_minimum_required(VERSION 3.8)

project(OpenGLRayTrace)

set (CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party)

add_executable(OpenGLRayTrace 
	OpenGLRayTrace.cpp
	OpenGLRayTrace.h
)

target_link_libraries(OpenGLRayTrace PUBLIC
	glew::glew
	glfw::glfw
	glm::glm
)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(OpenGLRayTrace PRIVATE /W4 /WX)
	string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    # lots of warnings and all warnings as errors
    target_compile_options(OpenGLRayTrace PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS OpenGLRayTrace DESTINATION bin)
install(DIRECTORY shaders DESTINATION ${CMAKE_INSTALL_PREFIX})