add_library(stb_image INTERFACE)

target_include_directories(stb_image INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/stb
)

add_library(stb::stb ALIAS stb_image)