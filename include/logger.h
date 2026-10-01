#pragma once

#include <iostream>

enum LogLevel {
	Error = 0, Warning, Info, Debug
};

void log_debug(const char* format, ...);
void log_info(const char* format, ...);
void log_warning(const char* format, ...);
void log_error(const char* format, ...);