cmake_minimum_required(VERSION 3.8)

project(OpenGLRayTrace)

add_subdirectory(third_party)

add_executable(OpenGLRayTrace 
	OpenGLRayTrace.cpp
	OpenGLRayTrace.h
)

target_link_libraries(OpenGLRayTrace PUBLIC
	GLEW::GLEW
	GLFW::GLFW
)

install(TARGETS OpenGLRayTrace DESTINATION bin)