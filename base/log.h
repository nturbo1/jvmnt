#ifndef BASE_LOG_H
#define BASE_LOG_H

enum class LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    FIXME
};

void log_write(LogLevel lvl, const char* file, int line, const char* fmt, ...);

#define LOG_FIXME(fmt, ...) \
    log_write(LogLevel::FIXME, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef DEBUG
#define LOG_DEBUG(fmt, ...) \
    log_write(LogLevel::DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) \
    do {} while (0);
#endif

#define LOG_INFO(fmt, ...) \
    log_write(LogLevel::INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    log_write(LogLevel::WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    log_write(LogLevel::ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_FATAL(fmt, ...) \
    log_write(LogLevel::FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif // BASE_LOG_H
