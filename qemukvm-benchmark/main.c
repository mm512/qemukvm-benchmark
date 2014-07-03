#include <stdio.h>
#include <string.h>
#include "util.h"
#include "zlib_compression.h"
#include "bzip2_compression.h"
#include "snappy_compression.h"

void usage(void)
{
    printf("Usage:\n\tqemukvm-benchmark [-l or -h] source_path\n-l - low compression\n-h high compression");
}

void get_input_name(char *file_name, char **argv)
{
    if (argv[1][0] == '-') {
        strcpy(file_name, argv[2]);
    } else {
        strcpy(file_name, argv[1]);
    }
}

int get_compression_level(char *argv)
{
    if (argv[0] == '-') {
        if (argv[1] == 'l') {
            return LOW_COMPRESSION;
        } else {
            return HIGH_COMPRESSION;
        }
    }

    return HIGH_COMPRESSION;
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
    int iterations = 1;
    int level = HIGH_COMPRESSION;
    FILE *infile;
    char input_file_name[100];

    puts("*** QEMU KVM performance benchmark ***\n");

    if (argc < 2) {
        puts("Too few arguments");
        usage();
        return 1;
    }

    get_input_name(input_file_name, argv);

    // Level.
    level = get_compression_level(argv[1]);
    if (level == LOW_COMPRESSION) {
        puts("Compression level set to low.");
    } else {
        puts("Compression level set to high.");
    }

    // Open input file.
    infile = fopen(input_file_name, "r");
    if (!infile) {
        puts("Error: problem with opening input file.");
        return 1;
    }


    puts("\nZLIB\n");
    run_benchmark(infile, input_file_name, LIB_ZLIB, level, iterations);
    rewind(infile);
    puts("\nBZIP2\n");
    run_benchmark(infile, input_file_name, LIB_BZIP2, level, iterations);
    rewind(infile);
    puts("\nSNAPPY\n");
    run_benchmark(infile, input_file_name, LIB_SNAPPY, level, iterations);

    fclose(infile);
    return 0;
}

