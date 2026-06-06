#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void genFile
(
    unsigned long long digit_count,
    int update_digit
);

void pretest_random_number(
    const char *filename, 
    unsigned long long num_digits
);