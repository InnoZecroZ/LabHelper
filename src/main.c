#include <stdio.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#include "Log/log.h"
#include "Timer/timer.h"
#include "CopyFile/copyfile.h"

/*
    Some Library are not available on all platforms.
*/
#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif


// ================================================================================================================================

// You can change this for some thing you want to change on debug mode
#define DEBUG_MODE false

// ================================================================================================================================

unsigned long long ffread(FILE *file, int buffersize){
    bool exit_loop = false;
    if (file != NULL)
    {
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
                fprintf(stderr,
                        "Warning: trailing chars after number: \"%s\"\n", end);
            }

            //printf("Read: \"%s\" (%zu btyes)\n", str, bytesRead);
            return result;
        }

        fprintf(stderr, "Failed to Read.\n");
        exit(1);
    }
    exit(1);
}

void read_text(FILE *pre_read, FILE *post_read){

    if (fseek(pre_read, 0, SEEK_END) != 0) {
        Logging("Error seeking");
        fclose(pre_read);
        return;
    }

    unsigned long long filesize = ftell(pre_read);

    if (filesize <= 0) {
        printf("File empty or error\n");
        fclose(pre_read);
        return;
    }

    const unsigned long long count = filesize;
    //printf("count: %llu\n", count);

    rewind(pre_read);

    int offset = count % 18 * - 1;

    //printf("Offset: %d\n", offset);

    /*
    int temp = floor(count / 18);
    temp = count - (temp * 18);
    printf("Temp: %d\n", temp);
    */  

    if ((count - 1) % 18 != 0)
    {
        //printf("\nOffset: %d\n", offset);

        if (fseek(pre_read, offset, SEEK_END) != 0) {
            Logging("Error seeking");
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

    const int buffersize = 18;

    char raw[buffersize];
    char str[buffersize + 1];
    size_t bytesRead;

    while (filesize > 0) {

        filesize -= 18;
        //printf("filesize : %llu\n", filesize);
        if (fseek(pre_read, filesize, SEEK_SET) != 0)
        {
            Logging("Error seeking");
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


void addition (FILE *file1, FILE *file2, FILE *file3){
    unsigned long long sum_of_char;     // ผลรวม Sum
    unsigned int carry = 0;             // The ทด
    unsigned long long Num1;            // Number from 1.txt
    unsigned long long Num2;            // Number from 2.txt
    bool last_num1 = false, last_num2 = false;


    /*
        Find the file size
    */

    // Move the cursor to end of files
    if (fseek(file1, 0, SEEK_END) != 0) {
        Logging("Error seeking");
        fclose(file1);
        return;
    }
    if (fseek(file2, 0, SEEK_END) != 0) {
        Logging("Error seeking");
        fclose(file2);
        return;
    }
    
    // Tell the cursor position
    unsigned long long filesize1 = ftell(file1);
    unsigned long long filesize2 = ftell(file2);
        
    // Check empty
    if (filesize1 <= 0) {
        printf("File empty or error\n");
        fclose(file1);
        return;
    }
    if (filesize2 <= 0) {
        printf("File empty or error\n");
        fclose(file2);
        return;
    }

    // Print file size (on Debug)
    //printf("filesize 1 : %llu\n", filesize1);
    //printf("filesize 2 : %llu\n", filesize2);



    int fd1 = fileno(file1);
    int fd2 = fileno(file2);

    if (fd1 == -1) {
        Logging("Error getting file descriptor");
        fclose(file1);
        return;
    }
    if (fd2 == -1) {
        Logging("Error getting file descriptor");
        fclose(file2);
        return;
    }
    
    while (file1 != NULL && file2 != NULL)
    {
        if (file1 != NULL)
        {
            if (filesize1 < 18)
            {
                rewind(file1);
                Num1 = ffread(file1, filesize1);
                filesize1 = 0;
                last_num1 = true;
            }
            else {
                filesize1 -= 18;
                if (fseek(file1, -18L, SEEK_END) != 0) {
                    Logging("Error seeking cur");
                    fclose(file1);
                    return;
                }

                /*
                if (fseek(file1, filesize1, SEEK_SET) != 0) {
                    Logging("Error seeking cur");
                    fclose(file1);
                    return;
                }
                */
                Num1 = ffread(file1, 18);
            }
            //printf("Num 1 : %llu\n", Num1);

            #ifdef _WIN32
                if (_chsize_s(fd1, filesize1) != 0) {
                    Logging("Error truncating file");
                    fclose(file1);
                    return;
                }
            #else
                if (ftruncate(fd1, filesize1) != 0) {
                    Logging("Error truncating file");
                    fclose(file1);
                    return;
                }
            #endif
                //printf("File truncated successfully.(1)\n\n");
        }

        if (file2 != NULL)
        {
            if (filesize2 < 18)
            {
                rewind(file2);
                Num2 = ffread(file2, filesize2);
                filesize2 = 0;
                last_num2 = true;
            }
            else {
                filesize2 -= 18;
                if (fseek(file2, -18L, SEEK_END) != 0) {
                    Logging("Error seeking cur");
                    fclose(file2);
                    return;
                }

                /*
                if (fseek(file2, filesize2, SEEK_SET) != 0) {
                    Logging("Error seeking cur");
                    fclose(file2);
                    return;
                }
                */
                Num2 = ffread(file2, 18);
            }
            //printf("Num 2 : %llu\n", Num2);

            #ifdef _WIN32
                if (_chsize_s(fd2, filesize2) != 0) {
                    Logging("Error truncating file");
                    fclose(file2);
                    return;
                }
            #else
                if (ftruncate(fd2, filesize2) != 0) {
                    Logging("Error truncating file");
                    fclose(file2);
                    return;
                }
            #endif
                //printf("File truncated successfully.(1)\n\n");
        }

        if (filesize1 == 0)
        {
            fclose(file1);
            file1 = NULL;
        }
        if (filesize2 == 0)
        {
            fclose(file2);
            file2 = NULL;
        }
        
        sum_of_char = Num1 + Num2 + carry;
        carry = 0;
        //printf("sum of char: %llu\n\n", sum_of_char);
        
        //maximum use with ull
        unsigned long long max_sum = 1000000000000000000;
        if (sum_of_char >= max_sum)
        {
            sum_of_char -= max_sum;
            carry = 1;
        }

        //printf("sum of char without carry: %llu\n", sum_of_char);
        
        if (last_num1 == false && last_num2 == false){
            //linear search
            unsigned long long digit = 0;
            for (unsigned long long i = 1; i <= 1000000000000000000ULL; digit++)
            {
                if (sum_of_char >= i)
                {
                    i *= 10;
                }
                else {
                    break;
                }
            }

            //printf("digit : %u\n", digit);
            for (size_t i = 18; i > digit; i--)
            {
                //add 0
                fprintf(file3, "0");
            }
        }

        fprintf(file3, "%llu", sum_of_char);
        sum_of_char = 0;
        Num1 = 0;
        Num2 = 0;
    }
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
        Logging("Error opening file");
        return 1;
    }
    if (!file2) {
        Logging("Error opening file");
        return 1;
    }

    FILE *file3 = fopen("unread-answer.txt", "w+");

    if (!file3) {
        Logging("Error opening file");
        return 1;
    }

    FILE *answer = fopen("answer.txt", "w");

    if (!answer) {
        Logging("Error opening file");
        return 1;
    }

    unsigned long start = mills();  // Start time measurement

    addition(file1, file2, file3);

    read_text(file3, answer);

    unsigned long end = mills(); // End time measurement
    printf("Elapsed time: %lu ms\n", end - start);
    return 0;
}