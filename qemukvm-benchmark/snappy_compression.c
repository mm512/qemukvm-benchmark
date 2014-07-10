#include "snappy_compression.h"
#include "util.h"
#include <snappy-c.h>
#include <stdlib.h>

static float mean_compression_time;
static float mean_compression_ratio;
static float mean_decompression_time;

/**
 * @brief Compresses source file to archive file. Measures compression stats.
 * @param source source file
 * @param arch archive file
 * @return Returns SNAPPY_SUCCESS on success or SNAPPY_FAILURE if something go wrong.
 */
int compress(FILE *source, FILE *arch)
{
    struct timespec start_ts, stop_ts;
    char *buffer;
    char *compressed;
    int buf_len;
    size_t compressed_len;

    buf_len = get_file_size(source);
    buffer = (char*)malloc(sizeof(char) * buf_len);
    if (!buffer) {
        puts("snappy compression error: problem with allocating memory for input buffer.");
        return SNAPPY_FAILURE;
    }

    compressed = (char*)malloc(sizeof(char) * snappy_max_compressed_length(buf_len));
    if (!compressed) {
        puts("snappy compression error: problem with allocating memory for archive buffer.");
        if (buffer) {
            free(buffer);
            buffer = NULL;
        }
        return SNAPPY_FAILURE;
    }

    // Start measure time.
    clock_gettime(CLOCK_REALTIME, &start_ts);

    fread(buffer, 1, buf_len, source);

    snappy_compress(buffer, buf_len, compressed, &compressed_len);

    fwrite(compressed, 1, compressed_len, arch);

    // Print/measure stats.
    clock_gettime(CLOCK_REALTIME, &stop_ts);
    struct timespec result_ts = diff(start_ts, stop_ts);
    mean_compression_time += result_ts.tv_nsec / 1000000.0f;
    mean_compression_ratio += (compressed_len / (float)buf_len) * 100.0f;

    if (buffer) {
        free(buffer);
        buffer = NULL;
    }

    if (compressed) {
        free(compressed);
        compressed = NULL;
    }
    return SNAPPY_SUCCESS;
}

/**
 * @brief Decompresses archive file and measures decompression stats.
 * @param arch archive file
 * @param output_file output, decompressed file
 * @return Returns SNAPPY_SUCCESS on success or SNAPPY_FAILURE if something go wrong.
 */
int decompress(FILE *arch, FILE *output_file)
{
    struct timespec start_ts, stop_ts;
    char *compressed = NULL;
    int compressed_len = 0;
    char *uncompressed = NULL;
    size_t uncompressed_len = 0;

    compressed_len = get_file_size(arch);
    compressed = (char*)malloc(sizeof(char) * compressed_len);
    if (!compressed) {
        puts("snappy decompression error: problem with allocating memory for archive buffer.");
        return SNAPPY_FAILURE;
    }

    clock_gettime(CLOCK_REALTIME, &start_ts);

    fread(compressed, 1, compressed_len, arch);
    snappy_uncompressed_length(compressed, compressed_len, &uncompressed_len);
    uncompressed = (char*)malloc(sizeof(char) * uncompressed_len);
    if (!uncompressed) {
        puts("snappy decompression error: problem with allocating memory for output buffer.");
        if (compressed) {
            free(compressed);
            compressed = NULL;
        }
        return SNAPPY_FAILURE;
    }

    snappy_uncompress(compressed, compressed_len, uncompressed, &uncompressed_len);

    if (fwrite(uncompressed, 1, uncompressed_len, output_file) != uncompressed_len || ferror(output_file)) {
        puts("snappy decompression error: problem with writing to output file");
        return SNAPPY_FAILURE;
    }

    clock_gettime(CLOCK_REALTIME, &stop_ts);

    struct timespec result_ts = diff(start_ts, stop_ts);
    mean_decompression_time += result_ts.tv_nsec / 1000000.0f;

    return SNAPPY_SUCCESS;
}

int run_snappy(FILE *source, FILE *arch, FILE *output, int iterations)
{
    int ret;
    for (int i = 0; i < iterations; ++i) {
        ret = compress(source, arch);
        if (ret == SNAPPY_FAILURE) {
            return ret;
        }

        rewind(source);
        rewind(arch);
    }

    printf("Mean compression ratio: %.2f%%\n", mean_compression_ratio / iterations);
    printf("Mean compression time: %.3f ms\n", mean_compression_time / iterations);

    rewind(arch);
    for (int i = 0; i < iterations; ++i) {
        ret = decompress(arch, output);
        if (ret == SNAPPY_FAILURE) {
            return ret;
        }

        rewind(arch);
    }

    printf("Mean decompression time: %.3f ms\n", mean_decompression_time / iterations);
    return SNAPPY_SUCCESS;
}
