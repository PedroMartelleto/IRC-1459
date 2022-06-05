#include "logger.h"
#include <stdarg.h>

std::mutex Logger::s_mutex;

void Logger::Print(const char* message, ...)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}
