#include <condition_variable>
#include <thread>
#include <queue>
#include <mutex>
#include <cmath>

#include "renderer.h"
#include "fractal.h"

struct RegionTask {
    Dimensions dimensions;
    Region region;
    FractalSettings settings;
};

static std::mutex result_mutex;
static Result result;
static bool result_available = false;

static std::condition_variable task_available_condition;
static Dimensions task_buffer;
static FractalSettings settings_buffer;
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
static void _calculator_worker();

void initialize_workers(bool asynchronous) {
    unsigned int thread_count = std::thread::hardware_concurrency();

    if (asynchronous) {
        worker_thread = std::thread(_worker);
    }

    for (unsigned int i = 0; i < thread_count; i++) {
        calculator_threads.emplace_back(_calculator_worker);
    }
}

void assign_task(const Dimensions &task, const FractalSettings &settings) {
    {
        std::lock_guard<std::mutex> lock(result_mutex);
        task_buffer = task;
        settings_buffer = settings;
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
        FractalSettings settings;
        {
            std::unique_lock<std::mutex> lock(result_mutex);

            task_available_condition.wait(lock, [] {
                return task_available;
            });

            task = task_buffer;
            settings = settings_buffer;
            task_available = false;
        }

        std::vector<Pixel> result_buffer = calculate_fractal(task, settings);

        {
            std::lock_guard<std::mutex> lock(result_mutex);
            result.dimensions = task;
            result.settings = settings;
            result.pixels = std::move(result_buffer);
            result_available = true;
        }
    }
}

std::vector<Pixel> calculate_fractal(const Dimensions &task, const FractalSettings &settings) {
    pixels.clear();
    pixels.resize(task.width * task.height);
    RegionTask worker_task;

    worker_task.settings = settings;
    worker_task.dimensions = task;
    tasks_remaining = NUM_REGIONS;
    for (unsigned int region_y = 0; region_y < regions_per_side; region_y++) {
        for (unsigned int region_x = 0; region_x < regions_per_side; region_x++) {
            std::lock_guard<std::mutex> lock(task_queue_mutex);

            worker_task.region.start_x = task.width * region_x / regions_per_side;
            worker_task.region.end_x = task.width * (region_x + 1) / regions_per_side;

            worker_task.region.start_y = task.height * region_y / regions_per_side;
            worker_task.region.end_y = task.height * (region_y + 1) / regions_per_side;

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

        calculate_region(task.dimensions, task.region, task.settings, pixels);

        {
            std::lock_guard<std::mutex> lock(tasks_remaining_mutex);
            tasks_remaining--;
            if (tasks_remaining == 0) {
                task_remaining_condition.notify_one();
            }
        }
    }
}
