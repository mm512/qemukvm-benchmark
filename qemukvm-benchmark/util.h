#ifndef UTIL_H
#define UTIL_H

#include <time.h>

#define STATS_MODE 1

enum {
    LIB_ZLIB
};
enum {
    LOW_COMPRESSION,
    HIGH_COMPRESSION
};

/**
 * @brief Calculates and returns difference between two timespec structures.
 * @param start Struct describing beginning of time measurement.
 * @param end Struct describing end of time measurement.
 * @return Returns structure describing difference between two other. That's the way to measure execution time.
 */
struct timespec diff(struct timespec start, struct timespec end);
#endif // UTIL_H
