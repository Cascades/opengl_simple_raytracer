add_subdirectory(camera)
add_subdirectory(window)
add_subdirectory(shaders)
add_subdirectory(filesystem)

add_executable(OpenGLRayTrace 
	OpenGLRayTrace.cpp
)

target_link_libraries(OpenGLRayTrace PUBLIC
	glew::glew
	glfw::glfw
	glm::glm
	stb::stb
	OpenGLRayTrace::Camera
	OpenGLRayTrace::FileSystem
	OpenGLRayTrace::Shaders
	OpenGLRayTrace::Window
)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(OpenGLRayTrace PRIVATE /W4 /WX /Za)
	string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    # lots of warnings and all warnings as errors
    target_compile_options(OpenGLRayTrace PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

include_what_you_use_property(OpenGLRayTrace)

install(TARGETS OpenGLRayTrace DESTINATION bin)