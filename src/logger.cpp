#include <cstdarg>
#include <chrono>
#include <cstdio>

#include "global.h"
#include "logger.h"

static const auto start_time = std::chrono::steady_clock::now();

void log_debug_impl(const char* format, ...) {
    const long long elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time
    ).count();

    std::fprintf(stdout, "[%lld ms] [DEBUG] ", elapsed);

    va_list args;
    va_start(args, format);
    std::vfprintf(stdout, format, args);
    va_end(args);

    std::fprintf(stdout, "\n");
}

void log_info_impl(const char* format, ...) {
    const long long elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time
    ).count();

    std::fprintf(stdout, "[%lld ms] [INFO] ", elapsed);

    va_list args;
    va_start(args, format);
    std::vfprintf(stdout, format, args);
    va_end(args);

    std::fprintf(stdout, "\n");
}

void log_warning_impl(const char* format, ...) {
    const long long elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time
    ).count();

    std::fprintf(stderr, "[%lld ms] [WARNING] ", elapsed);

    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);

    std::fprintf(stderr, "\n");
}

void log_error_impl(const char* format, ...) {
    const long long elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time
    ).count();

    std::fprintf(stderr, "[%lld ms] [ERROR] ", elapsed);

    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);

    std::fprintf(stderr, "\n");
}