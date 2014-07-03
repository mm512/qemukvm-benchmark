#ifndef BZIP2_COMPRESSION_H
#define BZIP2_COMPRESSION_H

#include <stdio.h>

enum {
    BZIP2_SUCCESS,
    BZIP2_FAILURE
};

/**
 * @brief Start benchmark with bzip2.
 * @param source input file
 * @param arch archive file after compression
 * @param compression_level compression level
 * @param iterations iterations count
 * @return Returns BZIP2_SUCCESS on success or BZIP2_FAILURE if something go wrong.
 */
int run_bzip2(FILE *source, FILE *arch, int compression_level, int iterations);

#endif // BZIP2_COMPRESSION_H
