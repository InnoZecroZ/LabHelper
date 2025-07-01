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

void Create_Thread(
    void* arg
)
{
    ThreadArgs* args = (ThreadArgs*)arg;
    const int thread_num = args->Thread_Num;
    const int Num_of_Thread = args->Num_of_Thread;

    pthread_t Thread_ID[Num_of_Thread];

    for (size_t i = 0; i < Num_of_Thread; i++)
    {
        args->Thread_Num = i;
        if (
            pthread_create(
                &Thread_ID[i], 
                NULL, 
                addition, 
                (void*)args
            )
            != 0
        )
        {
            Log(LOG_TYPE_ERROR, "Pthread", "Fail to Create");
        }    
    }
    pthread_join(Thread_ID[0], NULL);

    free(args);
}

