all: qemukvm-benchmark

qemukvm-benchmark: main.o util.o zlib_compression.o bzip2_compression.o snappy_compression.o
	gcc main.o util.o zlib_compression.o bzip2_compression.o snappy_compression.o -o qemukvm-benchmark -lrt -lz -lbz2 -lsnappy

main.o: main.c
	gcc -std=gnu99 -c main.c
	
util.o: util.c
	gcc -std=gnu99 -c util.c 
	
zlib_compression.o: zlib_compression.c
	gcc -std=gnu99 -c zlib_compression.c 
	
bzip2_compression.o: bzip2_compression.c
	gcc -std=gnu99 -c bzip2_compression.c
	
snappy_compression.o: snappy_compression.c
	gcc -std=gnu99 -c snappy_compression.c
clean:
	rm *.o qemukvm-benchmark