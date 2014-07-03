#include <stdlib.h>
#include <bzlib.h>
#include "bzip2_compression.h"
#include "util.h"


// If stats mode is enabled we're going to run
// several tests and measure mean values.
static float mean_compression_time;
static float mean_compression_ratio;
static float mean_decompression_time;

int get_file_size(FILE *input_file)
{
    int size = 0;
    fseek(input_file, 0, SEEK_END);
    size = ftell(input_file);
    rewind(input_file);

    return size;
}

int compress(FILE *source, FILE *arch, int level, unsigned int *source_len)
{
    char *buf;
    char *output;
    int buf_size;
    unsigned int output_size;
    int bz_error;
    struct timespec start_ts, stop_ts;

    buf_size = get_file_size(source);
    *source_len = buf_size; // Save input size.

    buf = (char*)malloc(sizeof(char) * buf_size);
    if (!buf) {
        puts("bzip2 error: problem with allocating memory for buffer.");
        return BZIP2_FAILURE;
    }

    // To guarantee that the compressed data will fit in its buffer,
    // allocate an output buffer of size 1% larger than the uncompressed data, plus six hundred extra bytes.
    output_size = buf_size + (1/100 * buf_size) + 600;
    output = (char*)malloc(sizeof(char) * output_size);

    // Start measure time.
    clock_gettime(CLOCK_REALTIME, &start_ts);

    fread(buf, 1, buf_size, source);

    bz_error = BZ2_bzBuffToBuffCompress(output, &output_size, buf, buf_size, level, 0, 0);

    if (bz_error != BZ_OK) {
        puts("bzip2 error: problems with compression.");
        return BZIP2_FAILURE;
    }

    fwrite(output, 1, output_size, arch);

    // Print/measure stats.
    clock_gettime(CLOCK_REALTIME, &stop_ts);
    struct timespec result_ts = diff(start_ts, stop_ts);
    #ifndef STATS_MODE
    printf("Handled bytes: %u, compressed bytes: %u\n", handled_bytes, compressed_bytes);
    printf("Compression ratio: %.2f%%\n", (output_size / (float)buf_size) * 100.0f);
    printf("Compression time: %.3f ms\n", result_ts.tv_nsec / 1000000.0f);
    #endif
    mean_compression_time += result_ts.tv_nsec / 1000000.0f;
    mean_compression_ratio += (output_size / (float)buf_size) * 100.0f;

    return BZIP2_SUCCESS;
}

int decompress(FILE *arch, unsigned int source_len)
{
    int bz_error;
    struct timespec start_ts, stop_ts;
    int arch_size = get_file_size(arch);
    char *input = (char*)malloc(sizeof(char) * arch_size);
    char *output = (char*)malloc(sizeof(char) * source_len);

    if (!input) {
        puts("bzip2 error: problem with allocating input buffer.");
        if (output) {
            free(output);
            output = NULL;
            return BZIP2_FAILURE;
        }
    }

    if (!output) {
        puts("bzip2 error: problem with allocating output buffer.");
        if (input) {
            free(input);
            input = NULL;
            return BZIP2_FAILURE;
        }
    }

    // Start measure time.
    clock_gettime(CLOCK_REALTIME, &start_ts);
    fread(input, 1, arch_size, arch);
    bz_error = BZ2_bzBuffToBuffDecompress(output, &source_len, input, arch_size, 0, 0);

    if (bz_error != BZ_OK) {
        puts("bzip2 error: problems with decompression.");
        return BZIP2_FAILURE;
    }
    clock_gettime(CLOCK_REALTIME, &stop_ts);

    if (input) {
        free(input);
        input = NULL;
    }
    if (output) {
        free(output);
        output = NULL;
    }

    struct timespec result_ts = diff(start_ts, stop_ts);
    #ifndef STATS_MODE
    printf("Decompression time: %.3f ms\n", result_ts.tv_nsec / 1000000.0f);
    #endif
    mean_decompression_time += result_ts.tv_nsec / 1000000.0f;

    return BZIP2_SUCCESS;
}

int run_bzip2(FILE *source, FILE *arch, int compression_level, int iterations)
{
    unsigned int source_len;
    int level, ret;
    if (compression_level == LOW_COMPRESSION) {
        level = 1;
    } else {
        level = 9;
    }

    printf("bzip2: compression level set on %d\n", level);

#ifdef STATS_MODE
    for (int i = 0; i < iterations; ++i) {
        ret = compress(source, arch, level, &source_len);
        if (ret == BZIP2_FAILURE) {
            return ret;
        }

        rewind(source);
        rewind(arch);
    }

    printf("Mean compression ratio: %.2f%%\n", mean_compression_ratio / iterations);
    printf("Mean compression time: %.3f ms\n", mean_compression_time / iterations);

    rewind(arch);
    for (int i = 0; i < iterations; ++i) {
        ret = decompress(arch, source_len);
        if (ret == BZIP2_FAILURE) {
            return ret;
        }

        rewind(arch);
    }

    printf("Mean decompression time: %.3f ms\n", mean_decompression_time / iterations);
#else
    ret = compress(source, arch, level, &source_len);
    if (ret == BZIP2_FAILURE) {
        return ret;
    }

    rewind(arch);
    ret = decompress(arch, source_len);
    if (ret == BZIP2_FAILURE) {
        return ret;
    }
#endif

    return BZIP2_SUCCESS;
}
