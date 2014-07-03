DIR=../qemukvm-benchmark
all: qemukvm-benchmark

qemukvm-benchmark: main.o util.o zlib_compression.o bzip2_compression.o snappy_compression.o
	gcc main.o util.o zlib_compression.o bzip2_compression.o snappy_compression.o -o qemukvm-benchmark -lrt -lz -lbz2 -lsnappy
	rm *.o

main.o: $(DIR)/main.c
	gcc -std=gnu99 -c $(DIR)/main.c
	
util.o: $(DIR)/util.c
	gcc -std=gnu99 -c $(DIR)/util.c 
	
zlib_compression.o: $(DIR)/zlib_compression.c
	gcc -std=gnu99 -c $(DIR)/zlib_compression.c 
	
bzip2_compression.o: $(DIR)/bzip2_compression.c
	gcc -std=gnu99 -c $(DIR)/bzip2_compression.c
	
snappy_compression.o: $(DIR)/snappy_compression.c
	gcc -std=gnu99 -c $(DIR)/snappy_compression.c
clean:
	rm *.o qemukvm-benchmark