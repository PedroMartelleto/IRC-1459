#pragma once

#include <mutex>

/**
 * @brief A thread-safe logger.
 */
class Logger
{
public:
    static void Print(const char* message, ...);
private:
    static std::mutex s_mutex;
};