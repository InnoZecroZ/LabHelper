#include "ParallelProcessing/parallel.h"
#include "../main.h"
#include "../Struct/TreadArgs.h"
#include "../Log/log.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

extern FILE* InputFile_1;
extern FILE* InputFile_2;
extern FILE* OutputFile;

extern int ThreadCount;

void Create_Thread(
    void* arg
)
{
    // TODO: Fix a arguments for threads
    // TODO: IDK What to fix and put please contact "kornpat"
    //int* ThreadID = malloc(sizeof(int));

    pthread_t ThreadWorker[ThreadCount];

    for (size_t i = 0; i < ThreadCount; i++)
    {
        int result = pthread_create(
            &ThreadWorker[i],
            NULL,
            addition,
            (void*)arg
        );

        if (result != 0)
        {
            Log(LOG_TYPE_ERROR, "Pthread", "Fail to Create Thread");
        }
    }

    pthread_join(ThreadWorker[0], NULL);

    free(arg);
}

