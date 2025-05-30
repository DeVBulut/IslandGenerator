cmake_minimum_required(VERSION 3.15)
project(IconGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5.1 COMPONENTS graphics window system REQUIRED)

# Add the icon generator executable
add_executable(icon_generator icon_generator.cpp)

# Link SFML
target_link_libraries(icon_generator PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
) 