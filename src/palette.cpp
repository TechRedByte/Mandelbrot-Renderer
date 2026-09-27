#include <vector>
#include <cmath>

#include "global.h"
#include "palette.h"

const std::vector<Pixel> GRAY_SCALE {
    {0, 0, 0},
    {255, 255, 255}
};

Pixel get_color(double smooth_iteration) {
    double palette_position = std::fmod(smooth_iteration, PALETTE_DENSITY)
    * GRAY_SCALE.size()
    / PALETTE_DENSITY;

    double integer_part;
    double fractional_part = std::modf(palette_position, &integer_part);
    unsigned int palette_index = static_cast<unsigned int>(integer_part);

    Pixel color;

    uint8_t red1 = GRAY_SCALE[palette_index].r;
    uint8_t red2 = GRAY_SCALE[(palette_index + 1) % GRAY_SCALE.size()].r;
    color.r = red1 + (red2 - red1) * fractional_part;

    uint8_t green1 = GRAY_SCALE[palette_index].g;
    uint8_t green2 = GRAY_SCALE[(palette_index + 1) % GRAY_SCALE.size()].g;
    color.g = green1 + (green2 - green1) * fractional_part;

    uint8_t blue1 = GRAY_SCALE[palette_index].b;
    uint8_t blue2 = GRAY_SCALE[(palette_index + 1) % GRAY_SCALE.size()].b;
    color.b = blue1 + (blue2 - blue1) * fractional_part;

    return color;
}