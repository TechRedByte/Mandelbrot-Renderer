#pragma once

#include <optional>

#include "global.h"

void initialize_workers(bool asynchronous);
void assign_task(Dimensions dimensions);
std::optional<Result> return_result();
std::vector<Pixel> calculate_fractal(Dimensions task);