#include "ParallelProcessing/parallel.h"
#include "../main.h"
#include "../Log/log.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

extern FILE* InputFile_1;
extern FILE* InputFile_2;
extern FILE* OutputFile;
extern unsigned long long filesize1;
extern unsigned long long filesize2;

extern int ThreadCount;

void Create_Thread(
    void* arg
)
{
    // Create array to store thread work structures
    ThreadWork* threadWorks = malloc(ThreadCount * sizeof(ThreadWork));
    if (!threadWorks) {
        Log(LOG_TYPE_ERROR, "Thread", "Failed to allocate memory for thread work structures");
        return;
    }

    // Calculate work distribution
    unsigned long long max_filesize = (filesize1 > filesize2) ? filesize1 : filesize2;
    unsigned long long work_per_thread = max_filesize / ThreadCount;
    unsigned long long remainder = max_filesize % ThreadCount;

    pthread_t* ThreadWorker = malloc(ThreadCount * sizeof(pthread_t));
    if (!ThreadWorker) {
        Log(LOG_TYPE_ERROR, "Thread", "Failed to allocate memory for thread array");
        free(threadWorks);
        return;
    }

    Log(LOG_TYPE_DEBUG, "Thread", "Creating threads for parallel processing");

    // Distribute work among threads
    for (int i = 0; i < ThreadCount; i++)
    {
        threadWorks[i].thread_id = i;
        threadWorks[i].start_pos1 = i * work_per_thread;
        threadWorks[i].end_pos1 = (i + 1) * work_per_thread;
        threadWorks[i].start_pos2 = i * work_per_thread;
        threadWorks[i].end_pos2 = (i + 1) * work_per_thread;
        threadWorks[i].carry_in = 0;
        threadWorks[i].carry_out = 0;

        // Last thread takes the remainder
        if (i == ThreadCount - 1) {
            threadWorks[i].end_pos1 += remainder;
            threadWorks[i].end_pos2 += remainder;
        }

        // Ensure we don't exceed file sizes
        if (threadWorks[i].end_pos1 > filesize1) threadWorks[i].end_pos1 = filesize1;
        if (threadWorks[i].end_pos2 > filesize2) threadWorks[i].end_pos2 = filesize2;

        char thread_info[128];
        snprintf(thread_info, sizeof(thread_info), "Thread %d: processing positions %llu-%llu", 
                 i, threadWorks[i].start_pos1, threadWorks[i].end_pos1);
        Log(LOG_TYPE_DEBUG, "Thread", thread_info);

        int result = pthread_create(
            &ThreadWorker[i],
            NULL,
            addition,
            &threadWorks[i]
        );

        if (result != 0)
        {
            char error_msg[64];
            snprintf(error_msg, sizeof(error_msg), "Failed to create thread %d", i);
            Log(LOG_TYPE_ERROR, "Pthread", error_msg);
        }
    }
    
    // Join all threads
    for (int i = 0; i < ThreadCount; i++)
    {
        void* thread_result;
        pthread_join(ThreadWorker[i], &thread_result);
        
        char join_msg[64];
        snprintf(join_msg, sizeof(join_msg), "Thread %d completed", i);
        Log(LOG_TYPE_DEBUG, "Thread", join_msg);
    }
    
    Log(LOG_TYPE_DEBUG, "Thread", "All threads joined successfully");
    
    free(ThreadWorker);
    free(threadWorks);
}

