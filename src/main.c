#include <stdio.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "Log/log.h"
#include "Timer/timer.h"
#include "CopyFile/copyfile.h"
#include "ParallelProcessing/parallel.h"
#include "main.h"

#ifdef _WIN32
    #include <io.h>
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// ================================================================================================================================

    // define the max digit per read/calculation
    #define MAX_DIGIT 18

// ================================================================================================================================

    // You can change this for some thing you want to change on debug mode
    #define DEBUG_MODE false

// ================================================================================================================================

extern FILE* InputFile_1;
extern FILE* InputFile_2;
extern FILE* OutputFile;
extern unsigned long long filesize1;
extern unsigned long long filesize2;
extern int ThreadCount;
extern pthread_mutex_t file_mutex;
extern pthread_mutex_t output_mutex;

/**
 *    Function to read a file in chunks of a specified size.
 *    It reads the file, converts the read string to an unsigned long long integer
 * 
 *    To move a file pointer to the end of the file, use `fseek(file, 0, SEEK_END)`.
 *    To get the current position of the file pointer, use `ftell(file)`.
 *    To reset the file pointer to the beginning of the file, use `rewind(file)`.
 */
unsigned long long ffread(FILE *file, int buffersize){
    bool exit_loop = false;
    if (file != NULL) {
        char raw[buffersize];
        char str[buffersize + 1];
        size_t bytesRead;

        while ((bytesRead = fread(raw, 1, buffersize, file)) > 0) {
            // Copy-and-null-terminate:
            memcpy(str, raw, bytesRead);
            str[bytesRead] = '\0';

            // Now you can safely use str as a C-string:
            
            errno = 0;
            char *end;
            unsigned long long result = strtoull(str, &end, 10);
            if (end == str) {
                fprintf(stderr, "No digits found in chunk: \"%s\"\n", str);
            } else if (errno == ERANGE) {
                fprintf(stderr, "Overflow converting \"%s\"\n", str);
            } else if (*end != '\0') {
                fprintf(stderr, "Warning: trailing chars after number: \"%s\"\n", end);
            }

            //printf("Read: \"%s\" (%zu btyes)\n", str, bytesRead);
            return result;
        }

        fprintf(stderr, "Failed to Read.\n");
        exit(1);
    }
    exit(1);
}



// TODO: REad Fiel can't read IDK Why but task is check this function
/**
 * 
 */
void read_text(FILE *pre_read, FILE *post_read){

    Log(
        LOG_TYPE_DEBUG,
        "File Read",
        "Starting to read file"
    );

    if (fseek(pre_read, 0, SEEK_END) != 0) {
        Log(
            LOG_TYPE_ERROR,
            "File Read Error",
            "Error seeking to end of file"
        );

        fclose(pre_read);
        return;
    }

    unsigned long long filesize = ftell(pre_read);

    if (filesize <= 0) {
        Log(
            LOG_TYPE_ERROR,
            "File Read Error",
            "File is empty or error"
        );
        perror("Filesize");
        fclose(pre_read);
        return;
    }

    const unsigned long long count = filesize;
    //printf("count: %llu\n", count);

    rewind(pre_read);

    int offset = count % MAX_DIGIT * - 1;

    //printf("Offset: %d\n", offset);

    /*
    int temp = floor(count / 18);
    temp = count - (temp * 18);
    printf("Temp: %d\n", temp);
    */  

    if ((count - 1) % MAX_DIGIT != 0) {
        //printf("\nOffset: %d\n", offset);

        if (fseek(pre_read, offset, SEEK_END) != 0) {
            Log(
                LOG_TYPE_ERROR,
                "File Read Error",
                "Error seeking to offset in file"
            );

            fclose(pre_read);
            return;
        }

        const int buffersize = -offset;
        char raw[buffersize];
        char str[buffersize + 1];
        size_t bytesRead;

        while ((bytesRead = fread(raw, 1, buffersize, pre_read)) > 0) {
            // Copy and null-terminate for safe string use
            memcpy(str, raw, bytesRead);
            str[bytesRead] = '\0';

            // Process as a regular string
            //printf("Chunk read: \"%s\"\n", str);
            fprintf(post_read, "%s", str);
        }
    }
    rewind(pre_read);

    //printf("filesize before offset : %llu\n", filesize);
    filesize += offset; //offset is a negative number
    //printf("filesize after offset : %llu\n", filesize);

    const int buffersize = MAX_DIGIT;
    char raw[buffersize];
    char str[buffersize + 1];
    size_t bytesRead;

    while (filesize > 0) {

        filesize -= MAX_DIGIT;
        //printf("filesize : %llu\n", filesize);
        if (fseek(pre_read, filesize, SEEK_SET) != 0) {
            Log(
                LOG_TYPE_ERROR,
                "File Read Error",
                "Error seeking to position in file"
            );

            fclose(pre_read);
            return;
        }

        size_t bytesRead = fread(raw, 1, buffersize, pre_read);

        // Copy and null-terminate for safe string use
        memcpy(str, raw, bytesRead);
        str[bytesRead] = '\0';

        // Process as a regular string
        //printf("Chunk read: \"%s\"\n", str);
        fprintf(post_read, "%s", str);
    }
    
    fclose(pre_read);
    pre_read = NULL;

    fclose(post_read);
    post_read = NULL;

    Log(
        LOG_TYPE_INFO,
        "File Read",
        "File read successfully"
    );
    return;
}


// ================================================================================================================================

void* addition (void* arg) {

    ThreadWork* work = (ThreadWork*)arg;
    
    char start_msg[128];
    snprintf(start_msg, sizeof(start_msg), "Thread %d starting addition", work->thread_id);
    Log(LOG_TYPE_DEBUG, "Addition", start_msg);

    // Create separate file handles for each thread to avoid conflicts
    FILE* local_file1 = fopen("1.txt", "r");
    FILE* local_file2 = fopen("2.txt", "r");
    
    if (!local_file1 || !local_file2) {
        Log(LOG_TYPE_ERROR, "Addition", "Failed to open local file handles");
        if (local_file1) fclose(local_file1);
        if (local_file2) fclose(local_file2);
        return NULL;
    }

    unsigned int carry = work->carry_in;
    unsigned long long current_pos1 = work->start_pos1;
    unsigned long long current_pos2 = work->start_pos2;
    
    // Buffer to store results from this thread
    char* result_buffer = malloc(((work->end_pos1 - work->start_pos1) / MAX_DIGIT + 1) * 20);
    if (!result_buffer) {
        Log(LOG_TYPE_ERROR, "Addition", "Failed to allocate result buffer");
        fclose(local_file1);
        fclose(local_file2);
        return NULL;
    }
    
    int buffer_offset = 0;
    
    // Process assigned work chunk
    while (current_pos1 < work->end_pos1 && current_pos2 < work->end_pos2) {
        unsigned long long Num1 = 0, Num2 = 0;
        
        // Read from file 1
        if (current_pos1 < work->end_pos1) {
            int read_size = (work->end_pos1 - current_pos1 < MAX_DIGIT) ? 
                           (work->end_pos1 - current_pos1) : MAX_DIGIT;
            
            pthread_mutex_lock(&file_mutex);
            fseek(local_file1, current_pos1, SEEK_SET);
            Num1 = ffread(local_file1, read_size);
            pthread_mutex_unlock(&file_mutex);
            
            current_pos1 += read_size;
        }
        
        // Read from file 2
        if (current_pos2 < work->end_pos2) {
            int read_size = (work->end_pos2 - current_pos2 < MAX_DIGIT) ? 
                           (work->end_pos2 - current_pos2) : MAX_DIGIT;
            
            pthread_mutex_lock(&file_mutex);
            fseek(local_file2, current_pos2, SEEK_SET);
            Num2 = ffread(local_file2, read_size);
            pthread_mutex_unlock(&file_mutex);
            
            current_pos2 += read_size;
        }
        
        // Perform addition
        unsigned long long result = Num1 + Num2 + carry;
        carry = 0;
        
        // Handle overflow
        unsigned long long max_sum = 1000000000000000000;
        if (result >= max_sum) {
            result -= max_sum;
            carry = 1;
        }
        
        // Store result in buffer
        int written = snprintf(result_buffer + buffer_offset, 20, "%llu", result);
        buffer_offset += written;
    }
    
    work->carry_out = carry;
    
    // Write results to output file (thread-safe)
    pthread_mutex_lock(&output_mutex);
    fprintf(OutputFile, "%s", result_buffer);
    fflush(OutputFile);
    pthread_mutex_unlock(&output_mutex);
    
    free(result_buffer);
    fclose(local_file1);
    fclose(local_file2);

    char finish_msg[64];
    snprintf(finish_msg, sizeof(finish_msg), "Thread %d finished addition", work->thread_id);
    Log(LOG_TYPE_DEBUG, "Addition", finish_msg);
    
    return NULL;
}


void Subtract (FILE *InputFile_1, FILE *InputFile_2, FILE *OutputFile) {
    /**
     * TODO: Make a function to subtract two numbers from two files.
     */
}

void Multiply (FILE *InputFile_1, FILE *InputFile_2, FILE *OutputFile) {
    /**
     * TODO: Make a function to multiply two numbers from two files.
     */
}

void Divide (FILE *InputFile_1, FILE *InputFile_2, FILE *OutputFile) {
    /**
     * TODO: Make a function to divide two numbers from two files.
     */
}


/*
================================================================================================================================
*/

/*
    Function main
*/

int main(int argc, char *argv[]) {

    if (DEBUG_MODE) {
        /*
            Move a 1 Million number from 1m-num-tester-1.txt and 1m-num-tester-2.txt to 1.txt and 2.txt
            This is for testing the addition function with 1 million numbers.
        */
        //CopyFile("1m-num-tester-1.txt", "1.txt");
        //CopyFile("1m-num-tester-2.txt", "2.txt");

        /*
            Move a test number from 1test.txt and 2test.txt to 1.txt and 2.txt
            This is for testing the addition function with small numbers.
        */
        //CopyFile("1test.txt", "1.txt");
        //CopyFile("2test.txt", "2.txt");

        /*
            Move a 4k number from 4k-Num1.txt and 4k-Num2.txt to 1.txt and 2.txt
            This is for testing the addition function with 4k numbers.
        */
        //CopyFile("4k-Num1.txt", "1.txt");
        //CopyFile("4k-Num2.txt", "2.txt");
    }

    // -------------------------------------------------------------------------------------------

    // Get number of CPU cores for optimal thread count
    #ifdef _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        ThreadCount = sysinfo.dwNumberOfProcessors;
    #else
        ThreadCount = sysconf(_SC_NPROCESSORS_ONLN);
    #endif
    
    // For small files, use fewer threads to avoid overhead
    unsigned long long max_filesize = (filesize1 > filesize2) ? filesize1 : filesize2;
    if (max_filesize < 100) {
        ThreadCount = 1;  // Use single thread for very small files
    } else if (max_filesize < 1000) {
        ThreadCount = (ThreadCount > 2) ? 2 : ThreadCount;  // Limit to 2 threads for small files
    } else {
        // Limit to reasonable number and ensure at least 1
        if (ThreadCount > 8) ThreadCount = 8;
    }
    
    if (ThreadCount < 1) ThreadCount = 1;
    
    char thread_msg[128];
    snprintf(thread_msg, sizeof(thread_msg), "Using %d threads for files of size %llu and %llu", 
             ThreadCount, filesize1, filesize2);
    Log(LOG_TYPE_INFO, "Threading", thread_msg);

    InputFile_1 = fopen("1.txt", "r+");
    InputFile_2 = fopen("2.txt", "r+");
    OutputFile = fopen("unread-answer.txt", "w+");

    if (!InputFile_1) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening 1.txt");
        exit(1);
    }
    if (!InputFile_2) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening 2.txt");
        exit(1);
    }
    if (!OutputFile) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening unread-answer.txt");
        exit(1);
    }
    
    // Move the cursor to end of files
    if (fseek(InputFile_1, 0, SEEK_END) != 0) {
        Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_1 to end");
        fclose(InputFile_1);
        exit(1);
    }
    if (fseek(InputFile_2, 0, SEEK_END) != 0) {
        Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_2 to end");
        fclose(InputFile_2);
        exit(1);
    }
    
    // Tell the cursor position
    filesize1 = ftell(InputFile_1);
    filesize2 = ftell(InputFile_2);
        
    // Check empty
    if (filesize1 <= 0) {
        printf("File empty or error\n");
        Log(LOG_TYPE_ERROR, "File Size", "InputFile_1 is empty or error");
        fclose(InputFile_1);
        exit(1);
    }
    if (filesize2 <= 0) {
        printf("File empty or error\n");
        Log(LOG_TYPE_ERROR, "File Size", "InputFile_2 is empty or error");
        fclose(InputFile_2);
        exit(1);
    }

    // Print file size (on Debug)
    if (DEBUG_MODE) {
        printf("filesize 1 : %llu\n", filesize1);
        printf("filesize 2 : %llu\n", filesize2);
    }

    unsigned long start = mills();  // Start time measurement

    Create_Thread(NULL);

    Log(LOG_TYPE_DEBUG, "Main", "Threads completed, now processing output");

    // Close OutputFile first and reopen in read mode
    fclose(OutputFile);
    OutputFile = NULL;
    
    Log(LOG_TYPE_DEBUG, "Main", "Opening files for read_text");
    
    FILE* unread_answer = fopen("unread-answer.txt", "r");
    FILE* answer = fopen("answer.txt", "w");

    if (!unread_answer) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening unread-answer.txt for reading");
        return 1;
    }
    
    if (!answer) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening answer.txt");
        if (unread_answer) fclose(unread_answer);
        return 1;
    }

    Log(LOG_TYPE_DEBUG, "Main", "About to call read_text");
    read_text(unread_answer, answer);

    unsigned long end = mills(); // End time measurement

    char perf_msg[128];
    snprintf(perf_msg, sizeof(perf_msg), "Elapsed time: %lu ms using %d threads", end - start, ThreadCount);
    Log(LOG_TYPE_INFO, "Performance", perf_msg);
    
    // Calculate throughput
    unsigned long long total_data = filesize1 + filesize2;
    double throughput = (double)total_data / (end - start) * 1000.0; // bytes per second
    char throughput_msg[128];
    snprintf(throughput_msg, sizeof(throughput_msg), "Throughput: %.2f bytes/second", throughput);
    Log(LOG_TYPE_INFO, "Performance", throughput_msg);
    
    return 0;
}