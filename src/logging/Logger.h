#pragma once

#include <Arduino.h>
#include "../Singleton.h"
#include "defines.h"

class CLogger : public CSingleton<CLogger> {
    friend class CSingleton<CLogger>;

private:
    CLogger() = default;

    ~CLogger() override = default;

public:
    void trace(const char *str, ...);

    void debug(const char *str, ...);

    void info(const char *str, ...);

    void warn(const char *str, ...);

    void error(const char *str, ...);

    void fatal(const char *str, ...);

    template<typename T>
    inline void traceArray(const char *str, const T *array, int size) {
        logArray(TRACE, str, array, size);
    }

    template<typename T>
    inline void debugArray(const char *str, const T *array, int size) {
        logArray(DEBUG, str, array, size);
    }

    template<typename T>
    inline void infoArray(const char *str, const T *array, int size) {
        logArray(INFO, str, array, size);
    }

    template<typename T>
    inline void warnArray(const char *str, const T *array, int size) {
        logArray(WARN, str, array, size);
    }

    template<typename T>
    inline void errorArray(const char *str, const T *array, int size) {
        logArray(ERROR, str, array, size);
    }

    template<typename T>
    inline void fatalArray(const char *str, const T *array, int size) {
        logArray(FATAL, str, array, size);
    }

private:
    enum Level {
        TRACE = LOG_LEVEL_TRACE,
        DEBUG = LOG_LEVEL_DEBUG,
        INFO = LOG_LEVEL_INFO,
        WARN = LOG_LEVEL_WARN,
        ERROR = LOG_LEVEL_ERROR,
        FATAL = LOG_LEVEL_FATAL
    };

    static const char *levelToString(Level level);

    void log(Level level, const char *str, va_list args);

    template<typename T>
    void logArray(Level level, const char *str, const T *array, int size) {
        if (level < LOG_LEVEL) {
            return;
        }

        Serial.printf("[%-5s] %s", levelToString(level), str);

        for (size_t i = 0; i < size; i++) {
            Serial.print(array[i]);
        }

        Serial.println();
    }
};
