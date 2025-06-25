#ifndef THREADARGS_H
#define THREADARGS_H

    #include <stdio.h>

    typedef struct {
        FILE *InputFile_1;
        FILE *InputFile_2;
        FILE *OutputFile;
    } ThreadArgs;

#endif // PARALLEL_H