set(INSTALL_ASSETSDIR "${CMAKE_INSTALL_BINDIR}/assets" CACHE PATH "Path to installed assets directory")
set(INSTALL_SHADERSDIR "${INSTALL_ASSETSDIR}/shaders" CACHE PATH "Path to installed shaders directory")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_INSTALL_BINDIR})