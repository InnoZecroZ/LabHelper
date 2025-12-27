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