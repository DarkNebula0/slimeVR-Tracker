#include "Logger.h"

void CLogger::trace(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(TRACE, format, args);
    va_end(args);
}

void CLogger::debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(DEBUG, format, args);
    va_end(args);
}

void CLogger::info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(INFO, format, args);
    va_end(args);
}

void CLogger::warn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(WARN, format, args);
    va_end(args);
}

void CLogger::error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(ERROR, format, args);
    va_end(args);
}

void CLogger::fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(FATAL, format, args);
    va_end(args);
}

void CLogger::log(Level level, const char *format, va_list args) {
    if (level < LOG_LEVEL) {
        return;
    }

    char buffer[256];
    vsnprintf(buffer, 256, format, args);

    Serial.printf("[%-5s]  %s\n", levelToString(level), buffer);
}

const char *CLogger::levelToString(Level level) {
    switch (level) {
        case TRACE:
            return "TRACE";
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}