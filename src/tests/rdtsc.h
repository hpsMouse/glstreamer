#ifndef _4FC15614_B7B1_11E2_8A39_206A8A22A96A
#define _4FC15614_B7B1_11E2_8A39_206A8A22A96A

#include <time.h>

inline double getTimeAsDouble()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    return now.tv_sec + now.tv_nsec * double(1e-9);
}

#endif
