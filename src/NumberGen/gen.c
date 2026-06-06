#include "gen.h"

void genFile
(
    unsigned long long digit_count,
    int update_digit
)
{
    // Check digit is not empty
    if (digit_count <= 0) {
        printf("Digit count must be greater than 0.\n");
        return;
    }
    
    /* Digit Report Updater */
    // Check Empty
    if (update_digit <= 0) {
        printf("Update digit must be greater than 0.\n");
        return;
    }
    
}

#define CHUNK_SIZE 4096


void pretest_random_number(
    const char *filename, 
    unsigned long long num_digits
) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Cannot open %s\n", filename);
        return;
    }

    // seed random
    srand((unsigned int)time(NULL));

    char buffer[CHUNK_SIZE + 1];
    unsigned long long written = 0;
    int buf_pos = 0;

    for (unsigned long long i = 0; i < num_digits; i++) {
        int digit;

        if (i == 0) {
            digit = (rand() % 9) + 1;  // หลักแรก 1-9
        } else {
            digit = rand() % 10;        // หลักถัดไป 0-9
        }

        buffer[buf_pos++] = '0' + digit;

        // เมื่อ buffer เต็ม → flush ลงไฟล์
        if (buf_pos == CHUNK_SIZE) {
            fwrite(buffer, 1, CHUNK_SIZE, f);
            buf_pos = 0;
        }
    }

    // เขียนที่เหลือ
    if (buf_pos > 0) {
        fwrite(buffer, 1, buf_pos, f);
    }

    fclose(f);
    printf("Generated %llu digits → %s\n", num_digits, filename);
}