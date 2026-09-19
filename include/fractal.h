#pragma once

#include <condition_variable>
#include <optional>
#include <vector>
#include <mutex>

#include "global.h"

void initialize_workers();
void assign_task(Dimensions dimensions);
std::optional<Result> return_result();
std::vector<Pixel> calculate_fractal(Dimensions task);