#pragma once

void log_debug_impl(const char* format, ...);
void log_info_impl(const char* format, ...);
void log_warning_impl(const char* format, ...);
void log_error_impl(const char* format, ...);

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_NONE 4

#define LOG_LEVEL LOG_LEVEL_DEBUG

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define log_debug(...) log_debug_impl(__VA_ARGS__)
#else
#define log_debug(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define log_info(...) log_info_impl(__VA_ARGS__)
#else
#define log_info(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define log_warning(...) log_warning_impl(__VA_ARGS__)
#else
#define log_warning(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define log_error(...) log_error_impl(__VA_ARGS__)
#else
#define log_error(...) ((void)0)
#endif