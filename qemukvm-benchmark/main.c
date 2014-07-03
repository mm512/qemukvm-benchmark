#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "zlib_compression.h"
#include "bzip2_compression.h"
#include "snappy_compression.h"

void usage(void)
{
    printf("Usage:\n\tqemukvm-benchmark [options] source_path\noptions:\n");
    printf("-l - low compression\n-h - high compression\n");
    printf("-t number - iterations\n");
    printf("--zlib - ZLIB compression\n");
    printf("--bzip2 - BZIP2 compression\n");
    printf("--snappy - Snappy compression\n");
    printf("--lzo - LZO compression\n\n");
}

void get_options(int argc, char **argv, bench_options *options, char *input_file_name)
{
    for (int i = 1; i < argc; ++i) {
        // Iterations
        if (!strcmp(argv[i], "-t")) {
            options->iterations = atoi(argv[i+1]);
        }
        // Compression
        else if (!strcmp(argv[i], "-l")) {
            options->level = LOW_COMPRESSION;
#ifndef STATS_MODE
            puts("Compression level set to low.");
#endif
        }
        else if (!strcmp(argv[i], "-h")) {
            options->level = HIGH_COMPRESSION;
#ifndef STATS_MODE
            puts("Compression level set to high.");
#endif
        }
        // Libraries
        else if (!strcmp(argv[i], "--zlib")) {
            options->library = LIB_ZLIB;
#ifndef STATS_MODE
            puts("Library set to zlib");
#endif
        }
        else if (!strcmp(argv[i], "--bzip2")) {
            options->library = LIB_BZIP2;
#ifndef STATS_MODE
            puts("Library set to bzip2");
#endif
        }
        else if (!strcmp(argv[i], "--snappy")) {
            options->library = LIB_SNAPPY;
#ifndef STATS_MODE
            puts("Library set to snappy");
#endif
        }
        else if (!strcmp(argv[i], "--lzo")) {
            options->library = LIB_LZO;
#ifndef STATS_MODE
            puts("Library set to lzo");
#endif
        }
        else {
            strcpy(input_file_name, argv[i]);
        }
    }
}

int run_benchmark(FILE *source, char *file_name, int library, int level, int iterations)
{
    FILE *archfile;
    char arch_file_name[100];
    strcpy(arch_file_name, file_name);

    switch(library) {
    case LIB_ZLIB:
        strcat(arch_file_name, ".zlib");
        archfile = fopen(arch_file_name, "w+");
        if (!archfile) {
            puts("Error: problem with opening archive file.");
            return 1;
        }
        run_zlib(source, archfile, level, iterations);
        fclose(archfile);
        break;
    case LIB_BZIP2:
        strcat(arch_file_name, ".bz2");
        archfile = fopen(arch_file_name, "w+");
        if (!archfile) {
            puts("Error: problem with opening archive file.");
            return 1;
        }
        run_bzip2(source, archfile, level, iterations);
        fclose(archfile);
        break;
    case LIB_SNAPPY:
        strcat(arch_file_name, ".snappy");
        archfile = fopen(arch_file_name, "w+");
        if (!archfile) {
            puts("Error: problem with openin archive file.");
            return 1;
        }

        run_snappy(source, archfile, iterations);
        fclose(archfile);
    default:
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    bench_options options;
    FILE *infile;
    char input_file_name[100];

    // Defaults.
    options.iterations = 1;
    options.level = HIGH_COMPRESSION;
    options.library = LIB_ZLIB;

#ifndef STATS_MODE
    puts("*** QEMU KVM performance benchmark ***\n");
#endif

    if (argc < 2) {
        puts("Too few arguments");
        usage();
        return 1;
    }

//    get_input_name(input_file_name, argv);

    get_options(argc, argv, &options, input_file_name);
#ifndef STATS_MODE
    printf("Iterations set to %d\n", options.iterations);
#endif

    // Open input file.
    infile = fopen(input_file_name, "r");
    if (!infile) {
        puts("Error: problem with opening input file.");
        return 1;
    }

    switch(options.library) {
    case LIB_ZLIB:
#ifndef STATS_MODE
        puts("\n\n*************** *************** ZLIB *************** ***************\n");
        printf("file: %s\n", input_file_name);
#endif
        run_benchmark(infile, input_file_name, LIB_ZLIB, options.level, options.iterations);
        rewind(infile);
        break;
    case LIB_BZIP2:
#ifndef STATS_MODE
        puts("\n\n*************** *************** BZIP2 *************** ***************\n");
        printf("file: %s\n", input_file_name);
#endif
        run_benchmark(infile, input_file_name, LIB_BZIP2, options.level, options.iterations);
        rewind(infile);
        break;
    case LIB_SNAPPY:
#ifndef STATS_MODE
        puts("\n\n*************** *************** SNAPPY *************** ***************\n");
        printf("file: %s\n", input_file_name);
#endif
        run_benchmark(infile, input_file_name, LIB_SNAPPY, options.level, options.iterations);
        break;
    case LIB_LZO:
    default:
        break;
    }

    fclose(infile);
    return 0;
}

