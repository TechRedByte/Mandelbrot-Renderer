cmake_minimum_required(VERSION 3.20)

project(mandelbrot_renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

add_executable(mandelbrot
    src/main.cpp
    src/fractal.cpp
)

target_include_directories(mandelbrot PRIVATE
    include
    ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(mandelbrot PRIVATE
    ${SDL3_LIBRARIES}
)