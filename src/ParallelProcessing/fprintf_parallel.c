#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif



#include "../Log/log.h"



#define MAX_DIGIT 18;
#define ULL_PLACEHOLDER ULLONG_MAX

extern unsigned long long* result;
extern int array_size;
extern bool LastNum;

void wait_nanoseconds(int nanosecond) {
    struct timespec ts;
    ts.tv_sec = nanosecond / 1000000;                // full seconds
    ts.tv_nsec = (nanosecond % 1) * 1;   // remaining ms → ns
    nanosleep(&ts, NULL);                           // pause the thread
}

void TruncateFile(
    FILE *file, 
    int fd, 
    unsigned long long filesize
)
{
    //printf("filesize : %llu\n", filesize);
    printf("fd : %d\n", fd);
    #ifdef _WIN32
        if (_chsize_s(fd, filesize) != 0) {
            Log(LOG_TYPE_ERROR, "File Truncate", "Error truncating InputFile_1");
            perror("chsize failed");
            fclose(file);
            return;
        }
    #else
        if (ftruncate(fd, filesize) != 0) {
            Log(LOG_TYPE_ERROR, "File Truncate", "Error truncating InputFile_1");
            fclose(file);
            return;
        }
    #endif
}

unsigned long long GetFileSize(FILE* file, int file_num){
    // Move the cursor to end of files
    if (fseek(file, 0, SEEK_END) != 0) {
        if (file_num == 1)
        {
            Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_1 to end");
        } else if (file_num == 2)
        {
            Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_2 to end");
        }
        
        fclose(file);
        exit(1);
    }
    
    // Tell the cursor position
    unsigned long long filesize = ftell(file);
    rewind(file);
        
    // Check empty
    if (filesize <= 0) {
        printf("File empty or error\n");
        if (file_num == 1)
        {
            Log(LOG_TYPE_ERROR, "File Size", "InputFile_1 is empty or error");
        } else if (file_num == 2)
        {
            Log(LOG_TYPE_ERROR, "File Size", "InputFile_2 is empty or error");
        } else {
            Log(LOG_TYPE_ERROR, "File Size, In valid Argument", "file is empty or error, ");
        }
        fclose(file);
        exit(1);
    }
}

void Print_Thread(
    unsigned int Num_of_Thread, 
    const char* filename1, 
    const char* filename2, 
    const char* filename3
)
{
    FILE *InputFile_1 = fopen(filename1, "r+b");
    FILE *InputFile_2 = fopen(filename2, "r+b");

    if (!InputFile_1) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening 1.txt");
        exit(1);
    }
    if (!InputFile_2) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening 2.txt");
        exit(1);
    }

    FILE *OutputFile = fopen(filename3, "w+");

    if (!OutputFile) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening unread-answer.txt");
        exit(1);
    }

    /**
     *   Get file descriptor
     *
     *   What is file descriptor?
     *   A file descriptor is a small integer value that the operating system uses 
     *   to identify and manage an open file. It is like an ID for an opened file,
     *   not the file's name.
     */

    // Fix file descriptor selection:
    #ifdef _WIN32
        int fd1 = _fileno(InputFile_1);
        int fd2 = _fileno(InputFile_2);
    #else
        int fd1 = fileno(InputFile_1);
        int fd2 = fileno(InputFile_2);
    #endif

    if (fd1 == -1) {
        Log(LOG_TYPE_ERROR, "File Descriptor", "Error getting file descriptor for InputFile_1");
        fclose(InputFile_1);
        exit(1);
    }
    if (fd2 == -1) {
        Log(LOG_TYPE_ERROR, "File Descriptor", "Error getting file descriptor for InputFile_2");
        fclose(InputFile_2);
        exit(1);
    }

    unsigned long long filesize1 = GetFileSize(InputFile_1, 1);
    unsigned long long filesize2 = GetFileSize(InputFile_2, 2);

    size_t i = 0;
    for (bool exit = false; exit == false; wait_nanoseconds(100))
    {
        if (result[i] != ULL_PLACEHOLDER)
        {
            printf("Result: %llu\n", result[i]);
            fprintf(OutputFile, "%llu", result[i]);
            result[i] = ULL_PLACEHOLDER;

            if (i + 1 == Num_of_Thread)
            {
                i = 0;
                
                if (LastNum == true)
                {
                    filesize1 = 0;
                    filesize2 = 0;
                    exit = true;
                }
                else {
                    filesize1 -= Num_of_Thread * MAX_DIGIT;
                    filesize2 -= Num_of_Thread * MAX_DIGIT;

                    // +++++++++++++++++++++++++ // ถ้า carry เป็น 1 ให้เพิ่ม 1 ให้ result[thread_num]
                    if (LastNum == false){
                        //linear search
                        unsigned long long digit = 0;
                        for (unsigned long long j = 1; j <= 1000000000000000000ULL; digit++)
                        {
                            if (result[i] >= j) {
                                j *= 10;
                            } else {
                                break;
                            }
                        }

                        //printf("digit : %u\n", digit);
                        
                        for (size_t j = 18; j > digit; j--)
                        {
                            //add 0
                            fprintf(OutputFile, "0");
                        }
                    }
                }
                
                TruncateFile(InputFile_1, fd1, filesize1);
                TruncateFile(InputFile_2, fd2, filesize2);
            }
        }
    }
}