#ifndef LZO_COMPRESSION_H
#define LZO_COMPRESSION_H


#include <stdio.h>

enum {
    LZO_SUCCESS,
    LZO_FAILURE
};

/**
 * @brief Start benchmark with LZO.
 * @param source input file
 * @param arch archive file after compression
 * @param output output file after decompression
 * @param compression_level compression level
 * @param iterations iterations count
 * @return Returns LZO_SUCCESS on success or LZO_FAILURE if something go wrong.
 */
int run_lzo(FILE *source, FILE *arch, FILE *output, int compression_level, int iterations);

#endif // LZO_COMPRESSION_H
