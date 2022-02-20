add_library(shaders INTERFACE)

target_include_directories(shaders INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(shaders INTERFACE
	glfw::glfw
	glew::glew
)

include_what_you_use_property(shaders)

add_library(OpenGLRayTrace::Shaders ALIAS shaders)