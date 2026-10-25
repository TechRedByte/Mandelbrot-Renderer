#pragma once

#include <optional>
#include <vector>

#include "global.h"

struct FractalSettings {
    unsigned int max_iterations = 1000;
    double palette_density = 60;
};

struct Result {
    std::vector<Pixel> pixels;
    unsigned int task_id;
};

void initialize_workers(bool asynchronous);
void assign_task(const Dimensions &task, const FractalSettings &settings, unsigned int task_id);
std::optional<Result> return_result();
std::vector<Pixel> calculate_fractal(const Dimensions &task, const FractalSettings &settings);