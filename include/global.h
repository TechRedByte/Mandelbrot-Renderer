#pragma once

#include <cstdint>

constexpr int MAX_ITERATIONS = 255;

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Dimensions {
    int width = 800;
    int height = 600;
    double scale = 0.003;
    double center_x = -0.75;
    double center_y = 0.0;
};