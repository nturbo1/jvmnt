#include "log.h"

#include <cstdio>
#include <cstdarg>
#include <stdexcept>

#ifdef DEBUG
static const LogLevel log_level = LogLevel::Debug;
#else
static const LogLevel log_level = LogLevel::Info;
#endif

static const char* lvl_name(LogLevel lvl)
{
    switch (lvl)
    {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Fatal:
            return "FATAL";
        case LogLevel::Fixme:
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

    if (lvl == LogLevel::Fatal)
        throw std::runtime_error("");
}
