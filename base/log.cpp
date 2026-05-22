#include "log.h"

#include <cstdio>
#include <cstdarg>
#include <stdexcept>

#ifdef DEBUG
static const LogLevel log_level = LogLevel::DEBUG;
#else
static const LogLevel log_level = LogLevel::INFO;
#endif

static const char* lvl_name(LogLevel lvl)
{
    switch (lvl)
    {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        case LogLevel::FIXME:
            return "FIXME";
        default:
            return "INFO";
    }
}

void log_write(LogLevel lvl, const char* file, int line, const char* fmt, ...)
{
    if (lvl < log_level)
    {
        return;
    }

    const char* lvlname = lvl_name(lvl);

    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[%s] %s:%d: ", lvlname, file, line);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);

    if (lvl == LogLevel::FATAL)
        throw std::runtime_error("");
}
