#include <cstdarg>
#include <chrono>
#include <cstdio>

#include "global.h"
#include "logger.h"

static const auto start_time = std::chrono::steady_clock::now();

void log_debug(const char* format, ...) {
    if (LOG_LEVEL >= Debug) {
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
}

void log_info(const char* format, ...) {
    if (LOG_LEVEL >= Info) {
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
}

void log_warning(const char* format, ...) {
    if (LOG_LEVEL >= Warning) {
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
}

void log_error(const char* format, ...) {
    if (LOG_LEVEL >= Error) {
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
}