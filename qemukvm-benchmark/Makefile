all: qemukvm-benchmark

qemukvm-benchmark: main.o util.o zlib_compression.o bzip2_compression
	gcc main.o util.o zlib_compression.o bzip2_compression.o -o qemukvm-benchmark -lrt -lz -lbz2

main.o: main.c
	gcc -c main.c
	
util.o: util.c
	gcc -c util.c 
	
zlib_compression.o: zlib_compression.c
	gcc -std=gnu99 -c zlib_compression.c 
	
bzip2_compression.o: bzip2_compression.c
	gcc -std=gnu99 -c bzip2_compression.c
