#include <stdio.h>
#include <string.h>
#include "util.h"
#include "zlib_compression.h"

void usage(void)
{
    printf("Usage:\n\tqemukvm-benchmark [-l or -h] source_path\n-l - low compression\n-h high compression");
}

int main(int argc, char **argv)
{
    int iterations = 1;
    int arg_index = 1;
    int level = HIGH_COMPRESSION;
    FILE *infile, *archfile;
    char arch_file_name[100];

    puts("*** QEMU KVM performance benchmark ***\n");

    if (argc < 2) {
        puts("Too few arguments");
        usage();
        return 1;
    }

    // Level.
    if (argv[1][0] == '-') {
        if (argv[1][1] == 'l') {
            level = LOW_COMPRESSION;
            puts("Compression level set to low.");
        } else {
            level = HIGH_COMPRESSION;
            puts("Compression level set to high.");
        }

        ++arg_index;
    } else {
        puts("Compression level set to high.");
    }

    infile = fopen(argv[arg_index], "r");
    if (!infile) {
        puts("Error: problem with opening input file.");
        return 1;
    }

    strcpy(arch_file_name, argv[arg_index]);
    strcat(arch_file_name, ".zlib");
    archfile = fopen(arch_file_name, "w+");
    if (!archfile) {
        puts("Error: problem with opening archive file.");
        fclose(infile);
        return 1;
    }

    run_zlib(infile, archfile, level, iterations);

    fclose(infile);
    fclose(archfile);
    return 0;
}

