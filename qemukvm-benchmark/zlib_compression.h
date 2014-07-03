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
 * @brief Start benchmark with zlib.
 * @param source input file
 * @param arch archive file after compression
 * @param compression_level compression level
 * @param iterations iterations count
 * @return Returns ZLIB_SUCCESS on success or ZLIB_FAILURE if something go wrong.
 */
int run_zlib(FILE *source, FILE *arch, int compression_level, int iterations);


#endif // ZLIB_COMPRESSION_H
