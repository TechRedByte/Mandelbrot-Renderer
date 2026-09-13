#pragma once

#include <condition_variable>
#include <optional>
#include <vector>
#include <mutex>

#include "global.h"

void initialize_worker();
void assign_task(Dimensions dimensions);
std::optional<std::vector<Pixel>> return_result();