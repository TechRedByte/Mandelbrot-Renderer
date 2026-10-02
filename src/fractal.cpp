#include <condition_variable>
#include <algorithm>
#include <optional>
#include <numbers>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <cmath>

#include "palette.h"
#include "fractal.h"
#include "global.h"

struct RegionTask {
    Dimensions dimensions;
    unsigned int start_x;
    unsigned int end_x;
    unsigned int start_y;
    unsigned int end_y;
};

static std::mutex result_mutex;
static Result result;
static bool result_available = false;

static std::condition_variable task_available_condition;
static Dimensions task_buffer;
static bool task_available = false;

static std::mutex task_queue_mutex;
static std::condition_variable task_queue_condition;
static std::queue<RegionTask> task_queue;

static std::mutex tasks_remaining_mutex;
static std::condition_variable task_remaining_condition;
static unsigned int tasks_remaining;

const unsigned int regions_per_side = std::sqrt(NUM_REGIONS);
static std::vector<std::thread> calculator_threads;
static std::thread worker_thread;
static std::vector<Pixel> pixels;

static void _worker();
static void _calculate_region(RegionTask task);
static void _calculator_worker();
static double _calculate_iterations(double real, double imaginary);
static void _color_pixel(Pixel &pixel, double smooth_iteration);
static bool _check_cardioid_and_period_2(double real, double imaginary);

void initialize_workers(bool asynchronous) {
    unsigned int thread_count = std::thread::hardware_concurrency();

    if (asynchronous) {
        worker_thread = std::thread(_worker);
    }

    for (unsigned int i = 0; i < thread_count; i++) {
        calculator_threads.emplace_back(_calculator_worker);
    }
}

void assign_task(Dimensions task) {
    {
        std::lock_guard<std::mutex> lock(result_mutex);
        task_buffer = task;
        task_available = true;
    }
    task_available_condition.notify_one();
}

std::optional<Result> return_result() {
    std::lock_guard<std::mutex> lock(result_mutex);
    if (result_available) {
        result_available = false;
        return result;
    } else {
        return std::nullopt;
    }
}

static void _worker() {
    while (true) {
        Dimensions task;
        {
            std::unique_lock<std::mutex> lock(result_mutex);

            task_available_condition.wait(lock, [] {
                return task_available;
            });

            task = task_buffer;
            task_available = false;
        }

        std::vector<Pixel> result_buffer = calculate_fractal(task);
        {
            std::lock_guard<std::mutex> lock(result_mutex);
            result.dimensions = task;
            result.pixels = std::move(result_buffer);
            result_available = true;
        }
    }
}

std::vector<Pixel> calculate_fractal(Dimensions task) {
    pixels.clear();
    pixels.resize(task.width * task.height);
    RegionTask worker_task;

    tasks_remaining = NUM_REGIONS;
    worker_task.dimensions = task;
    for (unsigned int region_y = 0; region_y < regions_per_side; region_y++) {
        for (unsigned int region_x = 0; region_x < regions_per_side; region_x++) {
            std::lock_guard<std::mutex> lock(task_queue_mutex);

            worker_task.start_x = task.width * region_x / regions_per_side;
            worker_task.end_x = task.width * (region_x + 1) / regions_per_side;

            worker_task.start_y = task.height * region_y / regions_per_side;
            worker_task.end_y = task.height * (region_y + 1) / regions_per_side;

            task_queue.push(worker_task);
        }
    }
    task_queue_condition.notify_all();

    {
        std::unique_lock<std::mutex> lock(tasks_remaining_mutex);
        task_remaining_condition.wait(lock, [] {
            return tasks_remaining == 0;
        });
    }

    return pixels;
}

static void _calculator_worker() {
    while (true) {
        RegionTask task;
        {
            std::unique_lock<std::mutex> lock(task_queue_mutex);

            task_queue_condition.wait(lock, [] {
                return !task_queue.empty();
            });

            task = task_queue.front();
            task_queue.pop();
        }
        _calculate_region(task);
        {
            std::lock_guard<std::mutex> lock(tasks_remaining_mutex);
            tasks_remaining--;
            if (tasks_remaining == 0) {
                task_remaining_condition.notify_one();
            }
        }
    }
}

static void _calculate_region(RegionTask task) {
    std::vector<double> imaginaries(task.end_y - task.start_y);
    std::vector<double> reals(task.end_x - task.start_x);

    for (unsigned int y = task.start_y; y < task.end_y; y++) {
        imaginaries[y - task.start_y] = task.dimensions.center_y + (y - task.dimensions.height / 2.0) * task.dimensions.scale;
    }
    for (unsigned int x = task.start_x; x < task.end_x; x++) {
        reals[x - task.start_x] = task.dimensions.center_x + (x - task.dimensions.width / 2.0) * task.dimensions.scale;
    }

    for (unsigned int y = 0; y < imaginaries.size(); y++) {
        for (unsigned int x = 0; x < reals.size(); x++) {
            Pixel &pixel = pixels[(y + task.start_y) * task.dimensions.width + (x + task.start_x)];

            if (_check_cardioid_and_period_2(reals[x], imaginaries[y])) {
                _color_pixel(pixel, MAX_ITERATIONS);
            } else {
                _color_pixel(pixel, _calculate_iterations(reals[x], imaginaries[y]));
            }
        }
    }
}

static double _calculate_iterations(double real, double imaginary) {
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

    return iteration + 1 - std::log(std::log(magnitude_squared) / 2.0) / std::numbers::ln2;
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

static bool _check_cardioid_and_period_2(double real, double imaginary) {
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