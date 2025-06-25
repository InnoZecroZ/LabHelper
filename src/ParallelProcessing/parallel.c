#include "ParallelProcessing/parallel.h"
#include "../main.h"
#include <pthread.h>
#include "../Struct/TreadArgs.h"
#include "../Log/log.h"

void Create_Thread(
    unsigned int Num_of_Thread, 
    FILE *InputFile_1, 
    FILE *InputFile_2, 
    FILE *OutputFile
)
{
    pthread_t Thread_ID[Num_of_Thread];

    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args->InputFile_1 = InputFile_1;
    args->InputFile_2 = InputFile_2;
    args->OutputFile = OutputFile;

    for (size_t i = 0; i < Num_of_Thread; i++)
    {
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
}

