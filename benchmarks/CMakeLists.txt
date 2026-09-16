cmake_minimum_required(VERSION 3.20)

project(mandelbrot_benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(benchmark
    main.cpp
    ../src/fractal.cpp
)

target_include_directories(benchmark PRIVATE
    ../include
)