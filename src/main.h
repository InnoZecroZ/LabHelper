#pragma once

    #include <stdio.h>
    #include <pthread.h>

    // Thread work structure
    typedef struct {
        int thread_id;
        unsigned long long start_pos1;
        unsigned long long end_pos1;
        unsigned long long start_pos2;
        unsigned long long end_pos2;
        unsigned long long carry_in;
        unsigned long long carry_out;
    } ThreadWork;

    void* addition(void* arg);               // Declaration of function
    
    // Thread synchronization variables
    extern pthread_mutex_t file_mutex;
    extern pthread_mutex_t output_mutex;