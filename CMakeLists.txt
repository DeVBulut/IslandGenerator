cmake_minimum_required(VERSION 3.15)
project(ProceduralIslandGenerator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Release by default for distribution
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add tools subdirectory
add_subdirectory(tools)

# Find SFML
find_package(SFML 2.5.1 COMPONENTS graphics window system REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# ImGui setup
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
add_library(ImGui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ImGui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SFML_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(ImGui PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    ${OPENGL_LIBRARIES}
)

# Add source files
set(SOURCES
    src/main.cpp
    src/NoiseGenerator.cpp
    src/IslandGenerator.cpp
    src/TextureManager.cpp
)

# Add header files
set(HEADERS
    include/NoiseGenerator.hpp
    include/IslandGenerator.hpp
    include/TextureManager.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ImGui
    ${OPENGL_LIBRARIES}
)

# Installation rules
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(TARGETS ImGui DESTINATION bin)

# Install SFML DLLs for Windows
if(WIN32)
    # Helper function to install DLLs
    function(install_dll_file DLL_PATH)
        if(EXISTS "${DLL_PATH}")
            install(FILES "${DLL_PATH}" DESTINATION bin)
        endif()
    endfunction()

    # Get SFML binary directory
    get_target_property(SFML_GRAPHICS_LOCATION sfml-graphics IMPORTED_LOCATION_RELEASE)
    get_filename_component(SFML_BIN_DIR "${SFML_GRAPHICS_LOCATION}" DIRECTORY)

    # Install SFML DLLs
    install_dll_file("${SFML_BIN_DIR}/sfml-graphics-2.dll")
    install_dll_file("${SFML_BIN_DIR}/sfml-window-2.dll")
    install_dll_file("${SFML_BIN_DIR}/sfml-system-2.dll")
    
    # Install OpenGL DLL if needed
    install_dll_file("${OPENGL_gl_LIBRARY}")
    
    # Install Visual C++ Runtime DLLs
    include(InstallRequiredSystemLibraries)
endif()

# CPack configuration
set(CPACK_PACKAGE_NAME "ProceduralIslandGenerator")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A procedural island generation tool")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ProceduralIslandGenerator")

# NSIS specific settings
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "Procedural Island Generator")
    set(CPACK_NSIS_DISPLAY_NAME "Procedural Island Generator")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
    
    # Create Start Menu shortcut
    set(CPACK_NSIS_MENU_LINKS
        "bin/${PROJECT_NAME}.exe" "Procedural Island Generator"
    )
endif()

# Include CPack
include(CPack) 