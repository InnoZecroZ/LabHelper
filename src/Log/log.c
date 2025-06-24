#include "log.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

void Log
(
    const char* type,
    const char* title,
    const char* message
)
{

    char LogMSG[1024];
    char DateTime[64];

    // Get current date and time with milliseconds
#ifdef _WIN32
    SYSTEMTIME st;
    GetLocalTime(&st);
    snprintf(DateTime, sizeof(DateTime), "%04d-%02d-%02d %02d:%02d:%02d",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    int millisec = st.wMilliseconds;
#else
    struct timeval tv;
    struct tm* tm_info;
    gettimeofday(&tv, NULL);
    time_t now_sec = tv.tv_sec;
    tm_info = localtime(&now_sec);
    strftime(DateTime, sizeof(DateTime), "%Y-%m-%d %H:%M:%S", tm_info);
    int millisec = tv.tv_usec / 1000;
#endif

    /**
     * Format the log message based on the type
     * This allows for easy identification of the log type
     */
    if(type == LOG_TYPE_INFO) {
        snprintf(LogMSG, sizeof(LogMSG), "[%s.%03d] [INFO] %s: %s", DateTime, millisec, title, message);
    } else if(type == LOG_TYPE_WARNING) {
        snprintf(LogMSG, sizeof(LogMSG), "[%s.%03d] [WARNING] %s: %s", DateTime, millisec, title, message);
    } else if(type == LOG_TYPE_ERROR) {
        snprintf(LogMSG, sizeof(LogMSG), "[%s.%03d] [ERROR] %s: %s", DateTime, millisec, title, message);
    } else if(type == LOG_TYPE_DEBUG) {
        snprintf(LogMSG, sizeof(LogMSG), "[%s.%03d] [DEBUG] %s: %s", DateTime, millisec, title, message);
    } else {
        snprintf(LogMSG, sizeof(LogMSG), "[%s.%03d] [UNKNOWN] %s: %s", DateTime, millisec, title, message);
    }

    /*
        Save a Log to file log.txt
    */
    FILE* log_file = fopen("output.log", "a");
    if (log_file == NULL) {
        return;
    }
    fprintf(log_file, "%s\n", LogMSG);
    fclose(log_file);



    
    /*
        Print to console
        This is useful for debugging and monitoring
        the program's execution in real-time.
    */
    printf("%s\n", LogMSG); // Also print to console




}