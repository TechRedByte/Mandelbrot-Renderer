#include <complex>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

#include "config.h"
#include "pixel.h"
#include "fractal.h"

std::mutex mutex;
std::condition_variable condition;
bool task_available = false;
bool result_available = false;

Dimensions dimensions;
std::vector<Pixel> pixels;

static Dimensions _task_buffer1;
static Dimensions _task_buffer2;

static std::thread _worker_thread;

static std::atomic<unsigned long> _task_generation = 0;

static void _worker();
static int _calculate_pixel(double real, double imaginary);
static std::vector<Pixel> _calculate_fractal(Dimensions task, unsigned long my_generation);

void initialize_worker() {
    _worker_thread = std::thread(_worker);
}

void assign_task(Dimensions task) {
    {
        std::lock_guard<std::mutex> lock(mutex);

        _task_buffer1 = task;

        task_available = true;
        _task_generation++;
    }

    condition.notify_one();
}

static void _worker() {
    while (true) {
        unsigned long my_generation = 0;

        {
            std::unique_lock<std::mutex> lock(mutex);

            condition.wait(lock, [] {
                return task_available;
            });

            _task_buffer2 = _task_buffer1;
            my_generation = _task_generation;
            task_available = false;
        }

        std::vector<Pixel> _result_buffer = _calculate_fractal(_task_buffer2, my_generation);

        if (!_result_buffer.empty()) {
            std::lock_guard<std::mutex> lock(mutex);

            if (my_generation == _task_generation) {
                pixels = std::move(_result_buffer);
                result_available = true;
            }
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

static std::vector<Pixel> _calculate_fractal(Dimensions task, unsigned long my_generation) {
    std::vector<Pixel> pixels(task.width * task.height);
    for (int y = 0; y < task.height; y++) {
        for (int x = 0; x < task.width; x++) {
            if (my_generation != _task_generation) {
                return {};
            }

            double real = task.center_x + (x - task.width / 2.0) * task.scale;
            double imaginary = task.center_y + (y - task.height / 2.0) * task.scale;

            int iterations = _calculate_pixel(real, imaginary);

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