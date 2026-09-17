#include <condition_variable>
#include <optional>
#include <complex>
#include <vector>
#include <thread>
#include <mutex>

#include "fractal.h"
#include "global.h"

static std::mutex mutex;
static std::condition_variable condition;
static bool task_available = false;
static bool result_available = false;
static std::vector<Pixel> pixels;
static Dimensions task_buffer;
static std::thread worker_thread;

static void _worker();
static int _calculate_pixel(double real, double imaginary);

void initialize_worker() {
    worker_thread = std::thread(_worker);
}

void assign_task(Dimensions task) {
    {
        std::lock_guard<std::mutex> lock(mutex);

        task_buffer = task;

        task_available = true;
    }

    condition.notify_one();
}

std::optional<std::vector<Pixel>> return_result() {
    std::lock_guard<std::mutex> lock(mutex);

    if (result_available) {
        result_available = false;
        return pixels;
    } else {
        return std::nullopt;
    }
}

static void _worker() {
    while (true) {
        Dimensions task;
        {
            std::unique_lock<std::mutex> lock(mutex);

            condition.wait(lock, [] {
                return task_available;
            });

            task = task_buffer;
            task_available = false;
        }

        std::vector<Pixel> result = calculate_fractal(task);
        {
            std::lock_guard<std::mutex> lock(mutex);
            pixels = std::move(result);
            result_available = true;
        }
    }
}

static int _calculate_pixel(double real, double imaginary) {
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

std::vector<Pixel> calculate_fractal(Dimensions task) {
    std::vector<Pixel> pixels(task.width * task.height);
    std::vector<double> reals(task.width);
    std::vector<double> imaginaries(task.height);
    for (int x = 0; x < task.width; x++) {
        reals[x] = task.center_x + (x - task.width / 2.0) * task.scale;
    }
    for (int y = 0; y < task.height; y++) {
        imaginaries[y] = task.center_y + (y - task.height / 2.0) * task.scale;
    }

    for (int y = 0; y < task.height; y++) {
        for (int x = 0; x < task.width; x++) {
            int iterations = _calculate_pixel(reals[x], imaginaries[y]);

            Pixel &pixel = pixels[y * task.width + x];

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
    return pixels;
}