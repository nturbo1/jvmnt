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

void logWrite(LogLevel lvl, const char* file, int line, const char* fmt, ...);

#define logFixme(fmt, ...) \
    logWrite(LogLevel::Fixme, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

# ifdef DEBUG
# define logDebug(fmt, ...) \
    logWrite(LogLevel::Debug, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
# else
# define logDebug(fmt, ...) \
    do {} while (0);
# endif

#define logInfo(fmt, ...) \
    logWrite(LogLevel::Info, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define logWarn(fmt, ...) \
    logWrite(LogLevel::Warn, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define logError(fmt, ...) \
    logWrite(LogLevel::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define logFatal(fmt, ...) \
    logWrite(LogLevel::Fatal, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif // BASE_LOG_H
