#pragma once

#include <vector>

#include "global.h"

struct Region {
    unsigned int start_x;
    unsigned int end_x;
    unsigned int start_y;
    unsigned int end_y;
};

void calculate_region(const Dimensions &dimensions, const Region &region, std::vector<Pixel> &pixels);