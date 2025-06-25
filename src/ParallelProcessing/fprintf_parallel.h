#ifndef FPRINTF_PARALLEL_H
#define FPRINTF_PARALLEL_H

    #include <stdio.h>

    void Print_Thread(
        unsigned int Num_of_Thread, 
        const char* filename1, 
        const char* filename2, 
        const char* filename3
    );

#endif // PARALLEL_H