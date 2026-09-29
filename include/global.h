#pragma once

#include <cstdint>
#include <vector>

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

    bool operator==(const Dimensions& other) const {
        return width == other.width &&
            height == other.height &&
            scale == other.scale &&
            center_x == other.center_x &&
            center_y == other.center_y;
    }
};

struct Result {
    Dimensions dimensions;
    std::vector<Pixel> pixels;
};

constexpr unsigned int MAX_ITERATIONS = 256;
constexpr unsigned int NUM_REGIONS = 361; // Must be a whole square number
constexpr double PALETTE_DENSITY = 255;
constexpr unsigned int LOW_RESOLUTION_SCALE = 4;