#ifndef LOG_H
#define LOG_H

    #include <stdio.h>
    #include <stdlib.h>

    /**
     * Log type use to define the output of log
     * INFO - General information to tell the user
     * WARNING - Warning message to tell the error that can be happen in the future
     * ERROR - Error message
     * DEBUG - Debugging information use to report values and states
     */
    #define LOG_TYPE_INFO "INFO"
    #define LOG_TYPE_WARNING "WARNING"
    #define LOG_TYPE_ERROR "ERROR"
    #define LOG_TYPE_DEBUG "DEBUG"

    void Log
    (
        const char* type,
        const char* title,
        const char* message
    );

#endif // LOG_H