#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <stdio.h>

#define STATS_MODE 1

enum {
    LIB_ZLIB,
    LIB_BZIP2,
    LIB_SNAPPY,
    LIB_LZO
};
enum {
    LOW_COMPRESSION,
    HIGH_COMPRESSION
};

typedef struct {
    int iterations;
    int library;
    int level;
} bench_options;

/**
 * @brief Calculates and returns difference between two timespec structures.
 * @param start Struct describing beginning of time measurement.
 * @param end Struct describing end of time measurement.
 * @return Returns structure describing difference between two other. That's the way to measure execution time.
 */
struct timespec diff(struct timespec start, struct timespec end);

/**
 * @brief Gets input file size.
 * @param input_file input file
 * @return Input file size.
 */
int get_file_size(FILE *input_file);
#endif // UTIL_H
