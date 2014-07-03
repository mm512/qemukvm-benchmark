// Compression levels: -1 to 9
// Z_DEFAULT_COMPRESSION equals -1,
// compromise between compression and speed - it is equivalent to level 6.
// Level 0 - no compression at all.

#ifndef ZLIB_COMPRESSION_H
#define ZLIB_COMPRESSION_H

#include <stdio.h>

enum {
    ZLIB_SUCCESS,
    ZLIB_FAILURE
};

// If stats mode is enabled we're going to run
// several tests and measure mean values.
static float mean_compression_time;
static float mean_compression_ratio;
static float mean_decompression_time;

// Hack for Windows/MS-DOS to avoid corruption of the input and output data.
// Source: www.zlib.net/zlib_how.html
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
    #include <fcntl.h>
    #include <io.h>
    #define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
    #define SET_BINARY_MODE(file)
#endif

// Buffer size for feeding data to and pulling data from zlib routines.
#define CHUNK 262144    // 256 KB

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
int def(FILE *source, FILE *dest, int level);

/**
 * @brief Decompress data from source file to dest file.
 * @param source input file
 * @return Returns Z_OK on success,
 * Z_MEM_ERROR if memeory could not be allocated,
 * Z_DATA_ERROR if the deflate data is invalid or incomplete,
 * ZVERSION_ERROR if the version of zlib.h and the version of the library linked do not match,
 * Z_ERRNO if there is an error reading or writing the files.
 */
int inf(FILE *source);

/**
 * @brief Runs def function and measure compression stats.
 * @param source input file
 * @param arch archive file
 * @param level compression level
 * @return
 */
int compress_with_zlib(FILE *source, FILE *arch, int level);

/**
 * @brief Runs inf function and measure decompression stats.
 * @param source input file
 * @return
 */
int decompress_with_zlib(FILE *source);

/**
 * @brief Start benchmark with zlib.
 * @param source input file
 * @param arch archive file after compression
 * @param compression_level compression level
 * @param iterations iterations count
 * @return Returns ZLIB_SUCCESS on success or ZLIB_FAILURE if something go wrong.
 */
int run_zlib(FILE *source, FILE *arch, int compression_level, int iterations);


#endif // ZLIB_COMPRESSION_H
