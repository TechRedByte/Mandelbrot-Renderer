#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdexcept>
#include <string>

#include "renderer.h"
#include "headless.h"
#include "global.h"

void create_image(const std::string &output_name, const Dimensions &dimension, const FractalSettings &settings) {
    initialize_workers(false);
    std::vector<Pixel> pixel = calculate_fractal(dimension, settings);
    if (!stbi_write_png(
        output_name.c_str(),
        dimension.width,
        dimension.height,
        int (3),
        pixel.data(),
        (dimension.width * sizeof(Pixel))
    )) {
        throw std::runtime_error("Failed to write png");
    }
}