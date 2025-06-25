#ifndef PARALLEL_H
#define PARALLEL_H

    #include <stdio.h>

    void Create_Thread(
        unsigned int Num_of_Thread, 
        FILE *InputFile_1, 
        FILE *InputFile_2, 
        FILE *OutputFile
    );

#endif // PARALLEL_H