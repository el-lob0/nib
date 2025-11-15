cmake_minimum_required(VERSION 3.10)
project(rotating_banana C)

set(CMAKE_C_STANDARD 11)

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Include directories
include_directories(${GLFW_INCLUDE_DIRS} ../include ${CMAKE_SOURCE_DIR})  # include/glad.h

# Add executable
add_executable(painter painter.c glad.c)

# Link libraries
target_link_libraries(painter ${GLFW_LIBRARIES} dl m)  # Linux

