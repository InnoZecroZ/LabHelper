#include <stdio.h> 
#include <pthread.h>
    
unsigned long long filesize1;
unsigned long long filesize2;

FILE* InputFile_1;
FILE* InputFile_2;
FILE* OutputFile;

int ThreadCount;
int ThreadID;

// Thread synchronization
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;

// Work distribution
typedef struct {
    int thread_id;
    unsigned long long start_pos1;
    unsigned long long end_pos1;
    unsigned long long start_pos2;
    unsigned long long end_pos2;
    unsigned long long carry_in;
    unsigned long long carry_out;
} ThreadWork;