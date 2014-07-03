#include <stdio.h>
#include <string.h>
#include "util.h"
#include "zlib_compression.h"

void usage(void)
{
    printf("Usage:\n\tqemukvm-benchmark [-level] source_path\n");
}

int main(int argc, char **argv)
{
    int iterations = 1;
    int arg_index = 1;
    int level;
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
        level = argv[1][1] - '0';
        ++arg_index;
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

    run_zlib(infile, archfile, HIGH_COMPRESSION, iterations);

    fclose(infile);
    fclose(archfile);
    return 0;
}

