#include "log.h"

void Log
(
    const char* message
)
{




    /*
        Save a Log to file log.txt
    */
    FILE* log_file = fopen("output.log", "a");
    if (log_file == NULL) {
        return;
    }
    fprintf(log_file, "%s\n", message);
    fclose(log_file);



    
    /*
        Print to console
        This is useful for debugging and monitoring
        the program's execution in real-time.
    */
    printf("%s\n", message); // Also print to console




}