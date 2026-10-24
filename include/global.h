#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "arbitrary.h"
#include "logger.h"

constexpr unsigned int MAX_ITERATIONS = 256;
constexpr unsigned int NUM_REGIONS = 361; // Must be a whole square number
constexpr double PALETTE_DENSITY = 255;
constexpr mpfr_prec_t GUARD_BITS = 4;

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Dimensions {
    int width = 800;
    int height = 600;
    MPFR scale = 0.004;
    MPFR center_x = -0.75;
    MPFR center_y = 0.0;
    mpfr_prec_t precision = MPFR_PREC_MIN;

    void update_precision(MPFR& new_scale) {
        MPFR magnitude = arbitrary::max(
            arbitrary::max(arbitrary::abs(center_x), arbitrary::abs(center_y)),
            new_scale
        );

        mpfr_prec_t new_precision = arbitrary::ceil(arbitrary::log2(magnitude / new_scale)).get_long() + GUARD_BITS;

        new_precision = std::max(new_precision, static_cast<mpfr_prec_t>(MPFR_PREC_MIN));

        log_debug("New precision: %ld", static_cast<long>(new_precision));

        if (new_precision <= MPFR_PREC_MAX) {
            scale.set_precision(new_precision);
            center_x.set_precision(new_precision);
            center_y.set_precision(new_precision);
            precision = new_precision;
        }
    }

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