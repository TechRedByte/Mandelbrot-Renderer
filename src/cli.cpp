#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <string>

#include "renderer.h"
#include "global.h"
#include "cli.h"

int create_image(const std::string &output_name) {
    initialize_workers(false);
    Dimensions dimension;
    std::vector<Pixel> pixel = calculate_fractal(dimension);
    if (stbi_write_png(
        output_name.c_str(),
        dimension.width,
        dimension.height,
        int (3),
        pixel.data(),
        (dimension.width * sizeof(Pixel))
    )) {
        return 0;
    } else {
        return 1;
    }
}