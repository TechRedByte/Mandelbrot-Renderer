#pragma once

#include <optional>
#include <vector>

#include "global.h"

struct FractalSettings {
    unsigned int max_iterations = 1000;
    double palette_density = 100;

    bool operator==(const FractalSettings &other) const {
        return max_iterations == other.max_iterations &&
            palette_density == other.palette_density;
    }
};

struct Result {
    std::vector<Pixel> pixels;
    Dimensions dimensions;
    FractalSettings settings;
};

void initialize_workers(bool asynchronous);
void assign_task(const Dimensions &dimensions, const FractalSettings &settings);
std::optional<Result> return_result();
std::vector<Pixel> calculate_fractal(const Dimensions &dimensions, const FractalSettings &settings);