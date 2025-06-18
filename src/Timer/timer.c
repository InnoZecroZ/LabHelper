#include "timer.h"

/*
    Function for Timer
*/
unsigned long mills() {
    return (unsigned long)(clock() * 1000 / CLOCKS_PER_SEC);
}