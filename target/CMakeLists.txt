cmake_minimum_required(VERSION 3.10)
project(example C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

add_executable(example
    example.c
)

target_include_directories(example PUBLIC
    ${GLFW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/glad/
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(example
    ${CMAKE_SOURCE_DIR}/libnib.a
    ${GLFW_LIBRARIES}
    dl
    m
    GL
)
