#include "ParallelProcessing/parallel.h"
#include "../main.h"
#include "../Struct/TreadArgs.h"
#include "../Log/log.h"
#include "fprintf_parallel.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void Create_Thread(
    unsigned int Num_of_Thread, 
    const char* filename1, 
    const char* filename2, 
    const char* filename3
)
{
    FILE *InputFile_1 = fopen(filename1, "r+");
    FILE *InputFile_2 = fopen(filename2, "r+");

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

    pthread_t Thread_ID[Num_of_Thread];

    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args->InputFile_1 = InputFile_1;
    args->InputFile_2 = InputFile_2;
    args->OutputFile = OutputFile;
    args->Num_of_Thread = Num_of_Thread;

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
    Print_Thread(Num_of_Thread, filename1, filename2, filename3);
    pthread_join(Thread_ID[0], NULL);

    free(args);

    fclose(InputFile_1);
    fclose(InputFile_2);
    fclose(OutputFile);
}

