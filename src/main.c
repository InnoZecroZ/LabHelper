#include <stdio.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include "Log/log.h"
#include "Timer/timer.h"
#include "CopyFile/copyfile.h"
#include "ParallelProcessing/parallel.h"
#include "Struct/TreadArgs.h"

#ifdef _WIN32
    #include <io.h>
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




/**
 * 
 */
void read_text(FILE *pre_read, FILE *post_read){

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

    return;
}


// ================================================================================================================================


void* addition (void* arg) {
    ThreadArgs* args = (ThreadArgs*)args;
    FILE *InputFile_1 = args->InputFile_1;
    FILE *InputFile_2 = args->InputFile_2;
    FILE *OutputFile = args->OutputFile;
    free(args);
    

    unsigned long long sum_of_char;     // ผลรวม Sum
    unsigned int carry = 0;             // The ทด
    unsigned long long Num1;            // Number from 1.txt
    unsigned long long Num2;            // Number from 2.txt

    bool last_num1 = false, last_num2 = false;

    /*
        Find the file size
    */

    // Move the cursor to end of files
    if (fseek(InputFile_1, 0, SEEK_END) != 0) {
        Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_1 to end");
        fclose(InputFile_1);
        return NULL;
    }
    if (fseek(InputFile_2, 0, SEEK_END) != 0) {
        Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_2 to end");
        fclose(InputFile_2);
        return NULL;
    }
    
    // Tell the cursor position
    unsigned long long filesize1 = ftell(InputFile_1);
    unsigned long long filesize2 = ftell(InputFile_2);
        
    // Check empty
    if (filesize1 <= 0) {
        printf("File empty or error\n");
        Log(LOG_TYPE_ERROR, "File Size", "InputFile_1 is empty or error");
        fclose(InputFile_1);
        return NULL;
    }
    if (filesize2 <= 0) {
        printf("File empty or error\n");
        Log(LOG_TYPE_ERROR, "File Size", "InputFile_2 is empty or error");
        fclose(InputFile_2);
        return NULL;
    }

    // Print file size (on Debug)
    if (DEBUG_MODE) {
        printf("filesize 1 : %llu\n", filesize1);
        printf("filesize 2 : %llu\n", filesize2);
    }




    /**
     *   Get file descriptor
     *
     *   What is file descriptor?
     *   A file descriptor is a small integer value that the operating system uses 
     *   to identify and manage an open file. It is like an ID for an opened file,
     *   not the file's name.
     */
    int fd1 = fileno(InputFile_1);
    int fd2 = fileno(InputFile_2);

    if (fd1 == -1) {
        Log(LOG_TYPE_ERROR, "File Descriptor", "Error getting file descriptor for InputFile_1");
        fclose(InputFile_1);
        return NULL;
    }
    if (fd2 == -1) {
        Log(LOG_TYPE_ERROR, "File Descriptor", "Error getting file descriptor for InputFile_2");
        fclose(InputFile_2);
        return NULL;
    }



    
    while (InputFile_1 != NULL && InputFile_2 != NULL) {
        if (InputFile_1 != NULL) {
            if (filesize1 < MAX_DIGIT) {   // <-- If the file size is less than 18(max digit), read the whole file
                rewind(InputFile_1); // Reset the file pointer to the beginning
                Num1 = ffread(InputFile_1, filesize1);
                filesize1 = 0;
                last_num1 = true;
            } else {                // <-- If the file size is greater than or equal to 18, read the last 18 bytes
                filesize1 -= MAX_DIGIT;
                if (fseek(InputFile_1, -MAX_DIGIT, SEEK_END) != 0) {
                    Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_1 to last 18 bytes");
                    fclose(InputFile_1);
                    return NULL;
                }

                /* <-- This is not needed, because we already seek to the end of the file
                if (fseek(InputFile_1, filesize1, SEEK_SET) != 0) {
                    Log("Error seeking cur");
                    fclose(InputFile_1);
                    return;
                }
                */
                Num1 = ffread(InputFile_1, MAX_DIGIT);
            }
            //printf("Num 1 : %llu\n", Num1); // <-- Debug

            #ifdef _WIN32
                if (_chsize_s(fd1, filesize1) != 0) {
                    Log(LOG_TYPE_ERROR, "File Truncate", "Error truncating InputFile_1");
                    fclose(InputFile_1);
                    return NULL;
                }
            #else
                if (ftruncate(fd1, filesize1) != 0) {
                    Log(LOG_TYPE_ERROR, "File Truncate", "Error truncating InputFile_1");
                    fclose(InputFile_1);
                    return NULL;
                }
            #endif
                //printf("File truncated successfully.(1)\n\n");
        }

        if (InputFile_2 != NULL) {
            if (filesize2 < MAX_DIGIT) {
                rewind(InputFile_2);
                Num2 = ffread(InputFile_2, filesize2);
                filesize2 = 0;
                last_num2 = true;
            } else {
                filesize2 -= MAX_DIGIT;
                if (fseek(InputFile_2, -MAX_DIGIT, SEEK_END) != 0) {
                    Log(LOG_TYPE_ERROR, "File Seek", "Error seeking InputFile_2 to last 18 bytes");
                    fclose(InputFile_2);
                    return NULL;
                }

                /*
                if (fseek(InputFile_2, filesize2, SEEK_SET) != 0) {
                    Log("Error seeking cur");
                    fclose(InputFile_2);
                    return;
                }
                */
                Num2 = ffread(InputFile_2, MAX_DIGIT);
            }
            //printf("Num 2 : %llu\n", Num2);

            #ifdef _WIN32
                if (_chsize_s(fd2, filesize2) != 0) {
                    Log(LOG_TYPE_ERROR, "File Truncate", "Error truncating InputFile_2");
                    fclose(InputFile_2);
                    return NULL;
                }
            #else
                if (ftruncate(fd2, filesize2) != 0) {
                    Log(LOG_TYPE_ERROR, "File Truncate", "Error truncating InputFile_2");
                    fclose(InputFile_2);
                    return NULL;
                }
            #endif
                //printf("File truncated successfully.(1)\n\n");
        }

        if (filesize1 == 0) {
            fclose(InputFile_1);
            InputFile_1 = NULL;
        }
        if (filesize2 == 0) {
            fclose(InputFile_2);
            InputFile_2 = NULL;
        }




        // +++++++++++++++++++++++++ // บวกตัวเลขทั้งสอง + carry
        sum_of_char = Num1 + Num2 + carry;
        carry = 0;
        //printf("sum of char: %llu\n\n", sum_of_char); // <-- Debug
        
        // +++++++++++++++++++++++++ // ถ้า sum_of_char มากกว่าหรือเท่ากับ 1000000000000000000 ให้ลบ 1000000000000000000 และเพิ่ม carry เป็น 1
        unsigned long long max_sum = 1000000000000000000;
        if (sum_of_char >= max_sum) {
            sum_of_char -= max_sum;
            carry = 1;
        }

        if (DEBUG_MODE) {
            printf("sum of char without carry: %llu\n", sum_of_char); // <-- Debug
        }
        
        // +++++++++++++++++++++++++ // ถ้า carry เป็น 1 ให้เพิ่ม 1 ให้ sum_of_char
        if (last_num1 == false && last_num2 == false){
            //linear search
            unsigned long long digit = 0;
            for (unsigned long long i = 1; i <= 1000000000000000000ULL; digit++)
            {
                if (sum_of_char >= i) {
                    i *= 10;
                } else {
                    break;
                }
            }

            //printf("digit : %u\n", digit);
            for (size_t i = MAX_DIGIT; i > digit; i--)
            {
                //add 0
                fprintf(OutputFile, "0");
            }
        }

        // +++++++++++++++++++++++++ // เขียนผลลัพธ์ลงไฟล์
        fprintf(OutputFile, "%llu", sum_of_char);
        sum_of_char = 0;
        Num1 = 0;
        Num2 = 0;
    }
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
int main() {

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


    FILE *file1 = fopen("1.txt", "r+");
    FILE *file2 = fopen("2.txt", "r+");

    if (!file1) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening 1.txt");
        return 1;
    }
    if (!file2) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening 2.txt");
        return 1;
    }

    FILE *file3 = fopen("unread-answer.txt", "w+");

    if (!file3) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening unread-answer.txt");
        return 1;
    }

    FILE *answer = fopen("answer.txt", "w");

    if (!answer) {
        Log(LOG_TYPE_ERROR, "File Open", "Error opening answer.txt");
        return 1;
    }

    unsigned long start = mills();  // Start time measurement

    (void)Create_Thread(1, file1, file2, file3);

    read_text(file3, answer);

    unsigned long end = mills(); // End time measurement

    char perf_msg[64];
    snprintf(perf_msg, sizeof(perf_msg), "Elapsed time: %lu ms", end - start);
    Log(LOG_TYPE_INFO, "Performance", perf_msg);
    
    return 0;
}