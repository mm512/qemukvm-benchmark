// Based on lzopack.c from library's examples.

#include "lzo_compression.h"
#include "util.h"
#include <lzo/lzoconf.h>
#include <lzo/lzoutil.h>
#include <lzo/lzo1x.h>
#include <stdlib.h>
#include <string.h>

// If stats mode is enabled we're going to run
// several tests and measure mean values.
static float mean_compression_time;
static float mean_compression_ratio;
static float mean_decompression_time;

static unsigned long total_in;
static unsigned long total_out;

static const unsigned char lzo_header[7] =
    { 0x00, 0xe9, 0x4c, 0x5a, 0x4f, 0xff, 0x1a };


/**
 * @brief Reads data from file to buffer
 * @param fp input file
 * @param buf data buffer
 * @param len buffer size
 * @param allow_eof flag - allows end of file
 * @return Returns number of read elements.
 */
static int xread(FILE *fp, lzo_voidp buf, lzo_uint32 len, lzo_bool allow_eof)
{
    lzo_uint32 l;

    l = (lzo_uint32)lzo_fread(fp, buf, len);

    if (l > len || (l != len && !allow_eof)) {
        puts("LZO error: problem with reading.");
        return LZO_FAILURE;
    }

    total_in += l;
    return l;
}

/**
 * @brief Writes data from buffer to file.
 * @param fp output file
 * @param buf data buffer
 * @param len buffer size
 */
static void xwrite(FILE *fp, const lzo_voidp buf, lzo_uint32 len)
{
    if (fp != NULL && lzo_fwrite(fp, buf, len) != len){
        puts("LZO error: problem with writing.");
    }

    total_out += (unsigned long) len;
}

/**
 * @brief Reads portable 32-bit integer from file.
 * @param fp input file
 * @return Returns 32-bit integer.
 */
static lzo_uint32 xread32(FILE *fp)
{
    unsigned char b[4];
    lzo_uint32 v;

    xread(fp, b, 4, 0);
    v = (lzo_uint32) b[3] << 0;
    v |= (lzo_uint32) b[2] << 8;
    v |= (lzo_uint32) b[1] << 16;
    v |= (lzo_uint32) b[0] << 24;

    return v;
}

/**
 * @brief Writes portable 32-bit integer to file.
 * @param fp output file
 * @param v 32-bit integer
 */
static void xwrite32(FILE *fp, lzo_xint v)
{
    unsigned char b[4];

    b[3] = (unsigned char) ((v >> 0) & 0xff);
    b[2] = (unsigned char) ((v >> 8) & 0xff);
    b[1] = (unsigned char) ((v >> 16) & 0xff);
    b[0] = (unsigned char) ((v >> 24) & 0xff);
    xwrite(fp, b, 4);
}

/**
 * @brief Gets character from file
 * @param fp file
 * @return Returns character from given file.
 */
int xgetc(FILE *fp)
{
    unsigned char c;
    xread(fp, (lzo_voidp) &c, 1, 0);
    return c;
}

/**
 * @brief Puts character into file.
 * @param fp file
 * @param c character to put
 */
void xputc(FILE *fp, int c)
{
    unsigned char cc = (unsigned char)(c & 0xff);
    xwrite(fp, (const lzo_voidp) &cc, 1);
}

/**
 * @brief Compresses source file into archive file.
 * @param source source file
 * @param arch archive file
 * @param level compression level
 * @param block_size block size
 * @return Returns LZO_SUCCESS on success or LZO_FAILURE if something go wrong.
 */
static int compress(FILE *source, FILE *arch, int level, lzo_uint32 block_size)
{
    struct timespec start_ts, stop_ts;
    int ret;
    lzo_bytep in = NULL;
    lzo_bytep out = NULL;
    lzo_voidp wrkmem = NULL;
    lzo_uint in_len, out_len;
    lzo_uint32 flags = 1;
    int method = 1;
    lzo_uint32 wrk_len;

    // Write LZO header, flags, compression level, block size
    xwrite(arch, lzo_header, sizeof(lzo_header));
    xwrite32(arch, flags);
    xputc(arch, method);
    xputc(arch, level);
    xwrite32(arch, block_size);

    // Allocations
    in = (lzo_bytep)malloc(sizeof(lzo_byte) * block_size);
    out = (lzo_bytep)malloc(sizeof(lzo_byte) * (block_size + block_size / 8 + 64 +3));
    if (level == 9) {
        wrk_len = LZO1X_999_MEM_COMPRESS;
    } else {
        wrk_len = LZO1X_1_MEM_COMPRESS;
    }

    wrkmem = (lzo_voidp)malloc(wrk_len);
    if (!in || !out || !wrkmem) {
        puts("LZO error: problem with allocations.");

        lzo_free(in);
        lzo_free(out);
        lzo_free(wrkmem);
        return LZO_FAILURE;
    }

    // Compression
    clock_gettime(CLOCK_REALTIME, &start_ts);
    while(1) {
        in_len = xread(source, in, block_size, 1);
        if (in_len == 0) {
            break;
        }

        if (level == 9) {
            ret = lzo1x_999_compress(in, in_len, out, &out_len, wrkmem);
        } else {
            ret = lzo1x_1_compress(in, in_len, out, &out_len, wrkmem);
        }

        if (ret != LZO_E_OK || out_len > in_len + in_len / 16 + 64 +3) {
            puts("LZO error: problem with compression.");
            lzo_free(in);
            lzo_free(out);
            lzo_free(wrkmem);
            return LZO_FAILURE;
        }

        xwrite32(arch, in_len);

        // Write compressed block.
        if (out_len < in_len) {
            xwrite32(arch, out_len);
            xwrite(arch, out, out_len);
        }
        // Not compressible - write uncompressed block.
        else {
            xwrite32(arch, in_len);
            xwrite(arch, in, in_len);
        }
    }

    // Write EOF marker.
    xwrite32(arch, 0);

    // Print/measure stats.
    clock_gettime(CLOCK_REALTIME, &stop_ts);
    struct timespec result_ts = diff(start_ts, stop_ts);
    #ifndef STATS_MODE
    printf("Compression ratio: %.2f%%\n", (total_out / (float)total_in) * 100.0f);
    printf("Compression time: %.3f ms\n", result_ts.tv_nsec / 1000000.0f);
    #endif
    mean_compression_time += result_ts.tv_nsec / 1000000.0f;
    mean_compression_ratio += (total_out / (float)total_in) * 100.0f;
    total_out = 0;
    total_in = 0;

    lzo_free(in);
    lzo_free(out);
    lzo_free(wrkmem);

    return LZO_SUCCESS;
}

/**
 * @brief Decompresses data from file and measures stats.
 * @param arch archive file
 * @param output output, decompressed file
 * @return Returns LZO_SUCCESS on success or LZO_FAILURE if something go wrong.
 */
static int decompress(FILE *arch, FILE *output)
{
    struct timespec start_ts, stop_ts;
    int ret;
    unsigned char m[sizeof(lzo_header)];
    lzo_uint32 flags;
    int method;
    int compression_level;
    lzo_uint32 block_size;
    lzo_bytep buf = NULL;
    lzo_uint32 buf_len;

    // Check LZO header, read flags and block size
    if (xread(arch, m, sizeof(lzo_header), 1) != sizeof(lzo_header) ||
            memcmp(m, lzo_header, sizeof(lzo_header)) != 0) {
        puts("LZO decompression error: problem with reading LZO header.");
        return LZO_FAILURE;
    }

    flags = xread32(arch);
    method = xgetc(arch);
    compression_level = xgetc(arch);
    if (method != 1) {
        puts("LZO decompression error: invalid method");
        return LZO_FAILURE;
    }
    block_size = xread32(arch);
    if (block_size < 1024 || block_size > 8*1024*1024L) {
        puts("LZO decompression error: invalid block size");
        return LZO_FAILURE;
    }

    // Allocations.
    buf_len = block_size + block_size / 16 + 64 +3;
    buf = (lzo_bytep)malloc(sizeof(lzo_byte)*buf_len);
    if (!buf) {
        puts("LZO decompression error: problem with allocation");
        return LZO_FAILURE;
    }

    // Decompression
    clock_gettime(CLOCK_REALTIME, &start_ts);
    while(1)
    {
        lzo_bytep in;
        lzo_bytep out;
        lzo_uint in_len, out_len;

        out_len = xread32(arch);
        // Exit on last block
        if (out_len == 0) {
            break;
        }

        in_len = xread32(arch);
        if (in_len > block_size || out_len > block_size || in_len == 0 || in_len > out_len) {
            puts("LZO decompression error: problem with block size - data corrupted");
            lzo_free(buf);
            return LZO_FAILURE;
        }

        // Place compressed block at the top of the buffer.
        in = buf + buf_len - in_len;
        out = buf;

        xread(arch, in, in_len, 0);

        if (in_len < out_len) {
            lzo_uint new_len = out_len;
            ret = lzo1x_decompress_safe(in, in_len, out, &new_len, NULL);
            if (ret != LZO_E_OK || new_len != out_len) {
                puts("LZO decompression error: compressed data violation");
                lzo_free(buf);
                return LZO_FAILURE;
            }
            xwrite(output, out, out_len);
        }
        else {
            xwrite(output, in, in_len);
        }
    }
    clock_gettime(CLOCK_REALTIME, &stop_ts);

    struct timespec result_ts = diff(start_ts, stop_ts);
    #ifndef STATS_MODE
    printf("Decompression time: %.3f ms\n", result_ts.tv_nsec / 1000000.0f);
    #endif
    mean_decompression_time += result_ts.tv_nsec / 1000000.0f;

    lzo_free(buf);
    return LZO_SUCCESS;
}

int run_lzo(FILE *source, FILE *arch, FILE *output, int compression_level, int iterations)
{
    int ret, opt_compression_level;
    lzo_uint32 opt_block_size = 256 * 1024L;

    if (compression_level == HIGH_COMPRESSION) {
        opt_compression_level = 9;
    } else {
        opt_compression_level = 1;
    }

    if (lzo_init() != LZO_E_OK) {
        puts("LZO error: lzo_init() failed.");
        return LZO_FAILURE;
    }

    printf("LZO: compression level set on %d\n", opt_compression_level);

#ifdef STATS_MODE
    for (int i = 0; i < iterations; ++i) {
        ret = compress(source, arch, opt_compression_level, opt_block_size);
        if (ret == LZO_FAILURE) {
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
        if (ret == LZO_FAILURE) {
            return ret;
        }

        rewind(arch);
    }

    printf("Mean decompression time: %.3f ms\n", mean_decompression_time / iterations);
#else
    ret = compress(source, arch, opt_compression_level, opt_block_size);
    if (ret == LZO_FAILURE) {
        return ret;
    }

    rewind(arch);
    ret = decompress(arch, output);
    if (ret == LZO_FAILURE) {
        return ret;
    }
#endif

    return LZO_SUCCESS;
}
