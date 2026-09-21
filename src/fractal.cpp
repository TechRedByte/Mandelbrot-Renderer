#include <condition_variable>
#include <algorithm>
#include <optional>
#include <complex>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <cmath>

#include "fractal.h"
#include "global.h"

struct PixelTask {
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
static std::queue<PixelTask> task_queue;

static std::mutex tasks_remaining_mutex;
static std::condition_variable task_remaining_condition;
static unsigned int tasks_remaining;

const unsigned int regions_per_side = std::sqrt(NUM_REGIONS);
static std::vector<std::thread> calculator_threads;
static std::thread worker_thread;
static std::vector<Pixel> pixels;

static void _worker();
static void _calculate_region(PixelTask task);
static void _calculator_worker();
static double _calculate_pixel(double real, double imaginary);
static void _color_pixel(Pixel &pixel, double smooth_iteration);

void initialize_workers() {
    worker_thread = std::thread(_worker);
    for (int i = 0; i < WORKER_THREADS; i++) {
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
    PixelTask worker_task;

    tasks_remaining = NUM_REGIONS;
    worker_task.dimensions = task;
    for (int region_y = 0; region_y < regions_per_side; region_y++) {
        for (int region_x = 0; region_x < regions_per_side; region_x++) {
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
        PixelTask task;
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

static void _calculate_region(PixelTask task) {
    std::vector<double> imaginaries(task.end_y - task.start_y);
    std::vector<double> reals(task.end_x - task.start_x);
    for (int y = task.start_y; y < task.end_y; y++) {
        imaginaries[y - task.start_y] = task.dimensions.center_y + (y - task.dimensions.height / 2.0) * task.dimensions.scale;
    }
    for (int x = task.start_x; x < task.end_x; x++) {
        reals[x - task.start_x] = task.dimensions.center_x + (x - task.dimensions.width / 2.0) * task.dimensions.scale;
    }
    for (int y = 0; y < imaginaries.size(); y++) {
        for (int x = 0; x < reals.size(); x++) {
            _color_pixel(
                pixels[(y + task.start_y) * task.dimensions.width + (x + task.start_x)],
                _calculate_pixel(reals[x], imaginaries[y])
            );
        }
    }
}

static double _calculate_pixel(double real, double imaginary) {
    std::complex<double> c(real, imaginary);
    std::complex<double> z(0.0, 0.0);

    unsigned int iteration;
    for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        z = z * z + c;

        if (std::norm(z) > 4.0) {
            break;
        }
    }
    if (iteration == MAX_ITERATIONS) {
        return MAX_ITERATIONS;
    }

    return iteration + 1 - std::log(std::log(std::abs(z))) / std::log(2.0);
}

static void _color_pixel(Pixel &pixel, double smooth_iteration) {
    if (smooth_iteration >= MAX_ITERATIONS) {
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;
    } else {
        double brightness = smooth_iteration * 255 / MAX_ITERATIONS;
        brightness = std::clamp(brightness, 0.0, 255.0);

        pixel.r = static_cast<uint8_t>(brightness);
        pixel.g = static_cast<uint8_t>(brightness);
        pixel.b = static_cast<uint8_t>(brightness);
    }
}