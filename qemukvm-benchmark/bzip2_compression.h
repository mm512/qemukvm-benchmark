#ifndef BZIP2_COMPRESSION_H
#define BZIP2_COMPRESSION_H

#include <stdio.h>

enum {
    BZIP2_SUCCESS,
    BZIP2_FAILURE
};

/**
 * @brief Gets input file size.
 * @param input_file input file
 * @return Input file size.
 */
int get_file_size(FILE *input_file);

/**
 * @brief Compresses source file to archive file. Measures compression stats. Function saves source data size.
 * @param source source file
 * @param arch archive file
 * @param level compression level
 * @param source_len source data size
 * @return Returns BZIP2_SUCCESS on success or BZIP2_FAILURE if something go wrong.
 */
int compress(FILE *source, FILE *arch, int level, unsigned int *source_len);

/**
 * @brief Decompresses archive file and measures decompression stats.
 * @param arch archive file
 * @param source_len source (uncompressed) data size. It is calculated in compress function.
 * @return Returns BZIP2_SUCCESS on success or BZIP2_FAILURE if something go wrong.
 */
int decompress(FILE *arch, unsigned int source_len);

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
