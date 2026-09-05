#include <complex>
#include <vector>

#include "config.h"
#include "pixel.h"

static int _calculate_fractal(double real, double imaginary) {
    std::complex<double> c(real, imaginary);
    std::complex<double> z(0.0, 0.0);

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        z = z * z + c;

        if (std::abs(z) > 2.0) {
            return iteration;
        }
    }

    return MAX_ITERATIONS;
}

void render_fractal(std::vector<Pixel> &pixels, unsigned int width, unsigned int height, float scale) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            double real = -0.75 + (x - width / 2.0) * scale;
            double imaginary = 0.0 + (y - height / 2.0) * scale;

            int iterations = _calculate_fractal(real, imaginary);

            Pixel &pixel = pixels[y * width + x];

            if (iterations == MAX_ITERATIONS) {
                pixel.r = 0;
                pixel.g = 0;
                pixel.b = 0;
            } else {
                uint8_t brightness = iterations * 255 / MAX_ITERATIONS;

                pixel.r = brightness;
                pixel.g = brightness;
                pixel.b = brightness;
            }
        }
    }
}