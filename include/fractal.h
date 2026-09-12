#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>

#include "pixel.h"

struct Dimensions {
    int width = 800;
    int height = 600;
    double scale = 0.003;
    double center_x = -0.75;
    double center_y = 0.0;
};

extern std::mutex mutex;
extern std::condition_variable condition;
extern bool task_available;
extern bool result_available;

extern Dimensions dimensions;
extern std::vector<Pixel> pixels;

void initialize_worker();
void assign_task(Dimensions dimensions);