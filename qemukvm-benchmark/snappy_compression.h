#ifndef SNAPPY_COMPRESSION_H
#define SNAPPY_COMPRESSION_H

#include <stdio.h>

enum {
    SNAPPY_SUCCESS,
    SNAPPY_FAILURE
};

/**
 * @brief Start benchmark with snappy.
 * @param source input file
 * @param arch archive file after compression
 * @param iterations iterations count
 * @return Returns SNAPPY_SUCCESS on success or SNAPPY_FAILURE if something go wrong.
 */
int run_snappy(FILE *source, FILE *arch, int iterations);

#endif // SNAPPY_COMPRESSION_H
