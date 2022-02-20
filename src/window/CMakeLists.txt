add_library(window INTERFACE)

target_include_directories(window INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(window INTERFACE
	glfw::glfw
	glew::glew
	glm::glm
	OpenGLRayTrace::Camera
)

include_what_you_use_property(window)

add_library(OpenGLRayTrace::Window ALIAS window)