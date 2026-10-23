#include <numbers>
#include <limits>
#include <vector>
#include <cmath>

#include "arbitrary.h"
#include "palette.h"
#include "fractal.h"
#include "global.h"

constexpr unsigned int DOUBLE_LIMIT = std::numeric_limits<double>::digits;

static void _mpfr_calculate_region(const Dimensions &dimensions, const Region &region, std::vector<Pixel> &pixels);
static double _mpfr_calculate_iterations(MPFR real, MPFR imaginary, mpfr_prec_t precision);
static bool _mpfr_check_cardioid_and_period2(MPFR real, MPFR imaginary);
static void _double_calculate_region(const Dimensions &dimensions, const Region &region, std::vector<Pixel> &pixels);
static double _double_calculate_iterations(double real, double imaginary);
static bool _double_check_cardioid_and_period2(double real, double imaginary);
static void _color_pixel(Pixel &pixel, double smooth_iteration);

void calculate_region(const Dimensions &dimensions, const Region &region, std::vector<Pixel> &pixels) {
    if (dimensions.precision <= DOUBLE_LIMIT) {
        _double_calculate_region(dimensions, region, pixels);
    } else {
        _mpfr_calculate_region(dimensions, region, pixels);
    }
}

// mpfr mode
static void _mpfr_calculate_region(const Dimensions &dimensions, const Region &region, std::vector<Pixel> &pixels) {
    std::vector<MPFR> imaginaries;
    std::vector<MPFR> reals;

    for (unsigned int y = region.start_y; y < region.end_y; y++) {
        imaginaries.emplace_back(dimensions.precision);
        imaginaries.back() = dimensions.center_y + (y - dimensions.height / 2.0) * dimensions.scale;
    }

    for (unsigned int x = region.start_x; x < region.end_x; x++) {
        reals.emplace_back(dimensions.precision);
        reals.back() = dimensions.center_x + (x - dimensions.width / 2.0) * dimensions.scale;
    }

    for (unsigned int y = 0; y < imaginaries.size(); y++) {
        for (unsigned int x = 0; x < reals.size(); x++) {
            Pixel &pixel = pixels[(y + region.start_y) * dimensions.width + (x + region.start_x)];

            if (_mpfr_check_cardioid_and_period2(reals[x], imaginaries[y])) {
                _color_pixel(pixel, MAX_ITERATIONS);
            } else {
                _color_pixel(pixel, _mpfr_calculate_iterations(reals[x], imaginaries[y], dimensions.precision));
            }
        }
    }
}

static double _mpfr_calculate_iterations(MPFR real, MPFR imaginary, mpfr_prec_t precision) {
    MPFR x = 0.0;
    MPFR y = 0.0;
    MPFR magnitude_squared(precision);
    unsigned int iteration;

    for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        MPFR x_new = x * x - y * y + real;
        MPFR y_new = 2.0 * x * y + imaginary;

        x = x_new;
        y = y_new;

        magnitude_squared = x * x + y * y;

        if (magnitude_squared > 128.0) {
            break;
        }
    }

    if (iteration == MAX_ITERATIONS) {
        return MAX_ITERATIONS;
    }

    double smooth_iteration = std::max(
        0.0,
        (iteration + 1.0 - arbitrary::log2(arbitrary::log2(magnitude_squared) / 2.0) / std::numbers::ln2).get_double()
    );

    return smooth_iteration;
}

static bool _mpfr_check_cardioid_and_period2(MPFR real, MPFR imaginary) {
    // Period-2 bulb check
    if ((real + 1.0) * (real + 1.0) + imaginary * imaginary <= 0.0625) {
        return true;
    }
    
    // Cardioid check
    MPFR q = (real - 0.25) * (real - 0.25) + imaginary * imaginary;

    if (q * (q + (real - 0.25)) <= 0.25 * imaginary * imaginary) {
        return true;
    }

    return false;
}

// double mode
static void _double_calculate_region(const Dimensions &dimensions, const Region &region, std::vector<Pixel> &pixels) {
    std::vector<double> imaginaries(region.end_y - region.start_y);
    std::vector<double> reals(region.end_x - region.start_x);

    for (unsigned int y = region.start_y; y < region.end_y; y++) {
        imaginaries[y - region.start_y] = dimensions.center_y.get_double() + (y - dimensions.height / 2.0) * dimensions.scale.get_double();
    }
    for (unsigned int x = region.start_x; x < region.end_x; x++) {
        reals[x - region.start_x] = dimensions.center_x.get_double() + (x - dimensions.width / 2.0) * dimensions.scale.get_double();
    }

    for (unsigned int y = 0; y < imaginaries.size(); y++) {
        for (unsigned int x = 0; x < reals.size(); x++) {
            Pixel &pixel = pixels[(y + region.start_y) * dimensions.width + (x + region.start_x)];

            if (_double_check_cardioid_and_period2(reals[x], imaginaries[y])) {
                _color_pixel(pixel, MAX_ITERATIONS);
            } else {
                _color_pixel(pixel, _double_calculate_iterations(reals[x], imaginaries[y]));
            }
        }
    }
}

static double _double_calculate_iterations(double real, double imaginary) {
    double x = 0.0;
    double y = 0.0;
    double magnitude_squared;
    unsigned int iteration;

    for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        double x_new = x * x - y * y + real;
        double y_new = 2.0 * x * y + imaginary;

        x = x_new;
        y = y_new;

        magnitude_squared = x * x + y * y;

        if (magnitude_squared > 128.0) {
            break;
        }
    }

    if (iteration == MAX_ITERATIONS) {
        return MAX_ITERATIONS;
    }

    double smooth_iteration = std::max(
        0.0,
        (iteration + 1.0 - arbitrary::log2(arbitrary::log2(magnitude_squared) / 2.0) / std::numbers::ln2).get_double()
    );

    return smooth_iteration;
}

static bool _double_check_cardioid_and_period2(double real, double imaginary) {
    // Period-2 bulb check
    if ((real + 1.0) * (real + 1.0) + imaginary * imaginary <= 0.0625) {
        return true;
    }
    
    // Cardioid check
    double q = (real - 0.25) * (real - 0.25) + imaginary * imaginary;

    if (q * (q + (real - 0.25)) <= 0.25 * imaginary * imaginary) {
        return true;
    }

    return false;
}

static void _color_pixel(Pixel &pixel, double smooth_iteration) {
    if (smooth_iteration >= MAX_ITERATIONS) {
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;
    } else {
        pixel = get_color(smooth_iteration);
    }
}