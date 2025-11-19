cmake_minimum_required(VERSION 3.10)
project(nib C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Include directories
include_directories(${GLFW_INCLUDE_DIRS} ../include ${CMAKE_SOURCE_DIR})  # include/glad.h


set(NIB_SRC
    ${CMAKE_SOURCE_DIR}/painter.c
    ${CMAKE_SOURCE_DIR}/buffer.c
    ${CMAKE_SOURCE_DIR}/display.c
)

add_library(nib STATIC ${NIB_SRC})

target_include_directories(nib PUBLIC include)




# Add executable
add_executable(painter painter.c glad.c)



# Link libraries
target_link_libraries(painter ${GLFW_LIBRARIES} dl m)  # Linux

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g -O1")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
