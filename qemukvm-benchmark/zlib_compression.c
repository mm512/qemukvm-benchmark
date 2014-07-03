#include "zlib_compression.h"
#include "util.h"
#include <string.h>
#include <zlib.h>

// If stats mode is enabled we're going to run
// several tests and measure mean values.
static float mean_compression_time;
static float mean_compression_ratio;
static float mean_decompression_time;

/**
 * @brief Compresses data from source file to dest file.
 * @param source input file
 * @param dest output file
 * @param level compression level (in the range of -1 to 9)
 * @return Returns Z_OK on success,
 * Z_MEM_ERROR if memeory could not be allocated,
 * ZVERSION_ERROR if the version of zlib.h and the version of the library linked do not match,
 * Z_ERRNO if there is an error reading or writing the files.
 */
static int def(FILE *source, FILE *dest, int level)
{
    int ret, flush;
    unsigned int have;  // Amount of data returned from deflate().
    z_stream stream;    // Is used to pass information to/from zlib routines.
    unsigned char in[CHUNK];    // Input buffer.
    unsigned char out[CHUNK];   // Output buffer.

    // Initialize zlib state.
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    ret = deflateInit(&stream, level);
    if (ret != Z_OK) {
        return ret;
    }

    // Start compression (until the end of file).
    do {
        stream.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            // If error occurred - free the allocated zlib state.
            deflateEnd(&stream);
            return Z_ERRNO;
        }

        // Check if end of file.
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        stream.next_in = in;

        // Run deflate until output buffer not full.
        do {
            stream.avail_out = CHUNK;
            stream.next_out = out;
            ret = deflate(&stream, flush);
            have = CHUNK - stream.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&stream);
                return Z_ERRNO;
            }
        } while (stream.avail_out == 0);

    } while (flush != Z_FINISH);

    // Measure/print compression ratio.
#ifndef STATS_MODE
    printf("Handled bytes: %lu, compressed bytes: %lu\n", stream.total_in, stream.total_out);
    printf("Compression ratio: %.2f%%\n", (stream.total_out / (float)stream.total_in) * 100.0f);
#endif
    mean_compression_ratio += (stream.total_out / (float)stream.total_in) * 100.0f;

    deflateEnd(&stream);
    return Z_OK;
}

/**
 * @brief Decompress data from source file to dest file.
 * @param source input file
 * @return Returns Z_OK on success,
 * Z_MEM_ERROR if memeory could not be allocated,
 * Z_DATA_ERROR if the deflate data is invalid or incomplete,
 * ZVERSION_ERROR if the version of zlib.h and the version of the library linked do not match,
 * Z_ERRNO if there is an error reading or writing the files.
 */
static int inf(FILE *source)
{
    int ret;
//    unsigned int have;
    z_stream stream;    // Is used to pass information to/from zlib routines.
    unsigned char in[CHUNK];    // Input buffer.
    unsigned char out[CHUNK];   // Output buffer.

    // Initialize zlib state.
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    ret = inflateInit(&stream);
    if (ret != Z_OK) {
        return ret;
    }

    // Start compression (until the end of file).
    do {
        stream.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            inflateEnd(&stream);
            return Z_ERRNO;
        }
        if (stream.avail_in == 0) {
            break;
        }
        stream.next_in = in;

        // Run inflate until output buffer not null.
        do {
            stream.avail_out = CHUNK;
            stream.next_out = out;

            // No need to adjust flush parameter - zlib format is self-terminating.
            ret = inflate(&stream, Z_NO_FLUSH);

            // But we need to pay atention to return value.
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&stream);
                return ret;
            }

//            have = CHUNK - stream.avail_out;
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                inflateEnd(&stream);
//                return Z_ERRNO;
//            }
        } while (stream.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd(&stream);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/**
 * @brief Runs def function and measure compression stats.
 * @param source input file
 * @param arch archive file
 * @param level compression level
 * @return Returns ZLIB_SUCCESS on success or ZLIB_FAILURE if something go wrong.
 */
static int compress_with_zlib(FILE *source, FILE *arch, int level)
{
    struct timespec start_ts, stop_ts;

    clock_gettime(CLOCK_REALTIME, &start_ts);
    if (def(source, arch, level) != Z_OK) {
        puts("zlib compression error.");
        return ZLIB_FAILURE;
    }
    clock_gettime(CLOCK_REALTIME, &stop_ts);

    // Measure/print stats.
    struct timespec result_ts = diff(start_ts, stop_ts);
#ifndef STATS_MODE
    printf("Compression time: %.3f ms\n", result_ts.tv_nsec / 1000000.0f);
#endif

    mean_compression_time += result_ts.tv_nsec / 1000000.0f;

    return ZLIB_SUCCESS;
}

/**
 * @brief Runs inf function and measure decompression stats.
 * @param source input file
 * @return Returns ZLIB_SUCCESS on success or ZLIB_FAILURE if something go wrong.
 */
static int decompress_with_zlib(FILE *source)
{
    struct timespec start_ts, stop_ts;

    clock_gettime(CLOCK_REALTIME, &start_ts);
    if (inf(source) != Z_OK) {
        puts("zlib decompression error.");
        return ZLIB_FAILURE;
    }
    clock_gettime(CLOCK_REALTIME, &stop_ts);

    // Measure/print stats.
    struct timespec result_ts = diff(start_ts, stop_ts);
#ifndef STATS_MODE
    printf("Decompression time: %.3f ms\n", result_ts.tv_nsec / 1000000.0f);
#endif

    mean_decompression_time += result_ts.tv_nsec / 1000000.0f;

    return ZLIB_SUCCESS;
}

int run_zlib(FILE *source, FILE *arch, int compression_level, int iterations)
{
    int ret;
    // Compression level must be Z_DEFAULT_COMPRESSION, or between 0 and 9, where 0 gives no compression at all.
    int level;
    if (compression_level == LOW_COMPRESSION) {
        level = 1;
    } else {
        level = 9;
    }

    SET_BINARY_MODE(source);
    SET_BINARY_MODE(arch);

    printf("zlib: compression level set on %d\n", level);
#ifdef STATS_MODE
    for (int i = 0; i < iterations; ++i) {
        ret = compress_with_zlib(source, arch, level);
        if (ret == ZLIB_FAILURE) {
            return ret;
        }

        rewind(source);
        rewind(arch);
    }

    printf("Mean compression ratio: %.2f%%\n", mean_compression_ratio / iterations);
    printf("Mean compression time: %.3f ms\n", mean_compression_time / iterations);

    rewind(arch);
    for (int i = 0; i < iterations; ++i) {
        ret = decompress_with_zlib(arch);
        if (ret == ZLIB_FAILURE) {
            return ret;
        }

        rewind(arch);
    }

    printf("Mean decompression time: %.3f ms\n", mean_decompression_time / iterations);
#else
    ret = compress_with_zlib(source, arch, level);
    if (ret == ZLIB_FAILURE) {
        return ret;
    }

    rewind(arch);
    ret = decompress_with_zlib(arch);
    if (ret == ZLIB_FAILURE) {
        return ret;
    }
#endif

    return ZLIB_SUCCESS;
}
