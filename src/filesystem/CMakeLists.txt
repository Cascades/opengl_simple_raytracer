add_library(filesystem INTERFACE)

target_include_directories(filesystem INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(filesystem INTERFACE
	INSTALL_BINDIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
	INSTALL_ASSETSDIR="${CMAKE_INSTALL_PREFIX}/${INSTALL_ASSETSDIR}"
	INSTALL_SHADERSDIR="${CMAKE_INSTALL_PREFIX}/${INSTALL_SHADERSDIR}"
)

message(STATUS "INSTALL_ASSETSDIR set to ${CMAKE_INSTALL_PREFIX}/${INSTALL_ASSETSDIR}")
message(STATUS "INSTALL_SHADERSDIR set to ${CMAKE_INSTALL_PREFIX}/${INSTALL_SHADERSDIR}")

include_what_you_use_property(filesystem)

add_library(OpenGLRayTrace::FileSystem ALIAS filesystem)