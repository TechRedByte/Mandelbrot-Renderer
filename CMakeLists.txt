cmake_minimum_required(VERSION 3.20)

project(mandelbrot_renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(MPFR REQUIRED mpfr)

# Source files
set(COMMON_SOURCES
    src/logger.cpp
    src/fractal.cpp
    src/palette.cpp
    src/renderer.cpp
    src/arbitrary.cpp
)

# Compiler warnings
set(COMMON_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wformat=2
)

# Main renderer
add_executable(mandelbrot
    src/main.cpp
    src/ui.cpp
    src/headless.cpp
    ${COMMON_SOURCES}
)

target_include_directories(mandelbrot PRIVATE
    include
    ${SDL3_INCLUDE_DIRS}
    ${MPFR_INCLUDE_DIRS}
)

target_link_libraries(mandelbrot PRIVATE
    ${SDL3_LIBRARIES}
    ${MPFR_LIBRARIES}
)

target_compile_options(mandelbrot PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)

# Benchmark executable
add_executable(benchmark
    benchmarks/main.cpp
    ${COMMON_SOURCES}
)

target_include_directories(benchmark PRIVATE
    include
    ${MPFR_INCLUDE_DIRS}
)

target_link_libraries(benchmark PRIVATE
    ${MPFR_LIBRARIES}
)

target_compile_options(benchmark PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)

# Tests
add_executable(test
    tests/main.cpp
    ${COMMON_SOURCES}
)

target_include_directories(test PRIVATE
    include
    ${MPFR_INCLUDE_DIRS}
)

target_link_libraries(test PRIVATE
    ${MPFR_LIBRARIES}
)

target_compile_options(test PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)