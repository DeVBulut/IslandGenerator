cmake_minimum_required(VERSION 3.15)
project(ProceduralIslandGenerator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

# Add header files
set(HEADERS
    include/NoiseGenerator.hpp
    include/IslandGenerator.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
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