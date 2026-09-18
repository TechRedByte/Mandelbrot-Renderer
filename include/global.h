#pragma once

#include <cstdint>

constexpr unsigned int MAX_ITERATIONS = 255;
constexpr unsigned int WORKER_THREADS = 10;
constexpr unsigned int NUM_REGIONS = 1521; // Must be a whole square number

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Dimensions {
    int width = 800;
    int height = 600;
    double scale = 0.004;
    double center_x = -0.75;
    double center_y = 0.0;
};