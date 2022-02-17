add_library(camera STATIC
	camera.cpp
)

target_include_directories(camera PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(camera
	glfw::glfw
	glew::glew
	glm::glm
)

add_library(OpenGLRayTrace::Camera ALIAS camera)