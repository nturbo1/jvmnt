#ifndef BASE_LOG_H
#define BASE_LOG_H

enum class LogLevel
{
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    Fixme
};

void log_write(LogLevel lvl, const char* file, int line, const char* fmt, ...);

#define log_fixme(fmt, ...) \
    log_write(LogLevel::Fixme, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

# ifdef DEBUG
# define log_debug(fmt, ...) \
    log_write(LogLevel::Debug, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
# else
# define log_debug(fmt, ...) \
    do {} while (0);
# endif

#define log_info(fmt, ...) \
    log_write(LogLevel::Info, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define log_warn(fmt, ...) \
    log_write(LogLevel::Warn, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define log_error(fmt, ...) \
    log_write(LogLevel::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define log_fatal(fmt, ...) \
    log_write(LogLevel::Fatal, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif // BASE_LOG_H
