#include "logger.h"
#include <stdarg.h>

std::mutex Logger::s_mutex;

void Logger::Print(const char* message, ...)
{
    s_mutex.lock();

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
    
    fflush(stdout);

    s_mutex.unlock();
}
