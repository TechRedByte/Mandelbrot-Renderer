cmake_minimum_required(VERSION 3.20)

project(mandelbrot_benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPFR REQUIRED mpfr)

add_executable(benchmark
    main.cpp
    ../src/logger.cpp
    ../src/fractal.cpp
    ../src/palette.cpp
    ../src/renderer.cpp
    ../src/arbitrary.cpp
)

target_include_directories(benchmark PRIVATE
    ../include
    ${MPFR_INCLUDE_DIRS}
)

target_link_libraries(benchmark PRIVATE
    ${MPFR_LIBRARIES}
)

target_compile_options(benchmark PRIVATE
    -Wall
    -Wextra
    -Wformat=2
)