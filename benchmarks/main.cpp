#include <algorithm>
#include <iostream>
#include <numeric>
#include <iomanip>
#include <string>
#include <chrono>
#include <cmath>

#include "fractal.h"
#include "global.h"

double mean(const std::vector<uint64_t>& durations);
double median(const std::vector<uint64_t>& durations);
double standard_deviation(const std::vector<uint64_t>& durations);

int main() {
    std::cout << std::fixed << std::setprecision(0);
    Dimensions cases[5];

    // Baseline
    cases[0] = {
        .width = 800,
        .height = 600,
        .scale = 0.004,
        .center_x = -0.75,
        .center_y = 0.0
    };

    // High resolution
    cases[1] = {
        .width = 3840,
        .height = 2880,
        .scale = 0.000833333333333,
        .center_x = -0.75,
        .center_y = 0.0
    };

    // Boundary-heavy
    cases[2] = {
        .width = 800,
        .height = 600,
        .scale = 0.0002,
        .center_x = -0.743643887037151,
        .center_y = 0.131825904205330
    };

    // Deep zoom
    cases[3] = {
        .width = 800,
        .height = 600,
        .scale = 0.00000001,
        .center_x = -0.743643887037151,
        .center_y = 0.131825904205330
    };

    // Small image
    cases[4] = {
        .width = 80,
        .height = 60,
        .scale = 0.04,
        .center_x = -0.75,
        .center_y = 0.0
    };

    std::vector<double> means;
    std::vector<double> medians;
    std::vector<uint64_t> minimums;
    std::vector<uint64_t> maximums;
    std::vector<double> standard_deviation_nums;
    for (unsigned int i = 0; i < std::size(cases); i++) {
        std::vector<uint64_t> durations;
        for (unsigned int b = 0; b < 100; b++) {
            auto start = std::chrono::steady_clock::now();
            calculate_fractal(cases[i]);
            auto end = std::chrono::steady_clock::now();
            durations.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
        }
        
        means.push_back(mean(durations));
        medians.push_back(median(durations));
        minimums.push_back(*std::min_element(durations.begin(), durations.end()));
        maximums.push_back(*std::max_element(durations.begin(), durations.end()));
        standard_deviation_nums.push_back(standard_deviation(durations));

        std::cout << (100 / std::size(cases) * (i + 1)) << "\% done" << "\n";
    }

    std::cout << "\n\nReport:\n\n";

    const char* names[] = {
        "Baseline",
        "High resolution",
        "Boundary-heavy",
        "Deep zoom",
        "Small image"
    };

    for (unsigned int i = 0; i < std::size(cases); i++) {
        std::cout << names[i] << ":\n";
        std::cout << "  Mean:               " << means[i] << " μs\n";
        std::cout << "  Median:             " << medians[i] << " μs\n";
        std::cout << "  Minimum:            " << minimums[i] << " μs\n";
        std::cout << "  Maximum:            " << maximums[i] << " μs\n";
        std::cout << "  Standard deviation: " << standard_deviation_nums[i] << " μs\n\n";
    }

    return 0;
}

double mean(const std::vector<uint64_t>& durations) {
    return std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
}

double median(const std::vector<uint64_t>& durations) {
    std::vector<uint64_t> sorted_durations = durations;
    std::sort(sorted_durations.begin(), sorted_durations.end());
    if (sorted_durations.size() % 2 == 0) {
        std::vector<uint64_t> middle_values = {sorted_durations[sorted_durations.size() / 2 - 1], sorted_durations[sorted_durations.size() / 2]};
        return mean(middle_values);
    } else {
        return sorted_durations[sorted_durations.size() / 2];
    }
}

double standard_deviation(const std::vector<uint64_t>& durations) {
    double mean_num = mean(durations);
    double summed_difference = 0.0;
    for (unsigned int i = 0; i < durations.size(); i++) {
        summed_difference += (durations[i] - mean_num) * (durations[i] - mean_num);
    }
    return std::sqrt(summed_difference / durations.size());
}