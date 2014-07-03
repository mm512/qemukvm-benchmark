qemukvm-benchmark
=================

C performance benchmark for QEMU with TCG and KVM acceleration.

This simple benchmark is based on several compression libraries. 
It uses some standard compression test data sets from [Archive Comparison Test](http://compression.ca/act/act-win.html).

I run this benchmark on base Debian system inside QEMU, with no acceleration (QEMU's [Tiny Code Generator](http://wiki.qemu.org/Documentation/TCG)) and with [KVM](http://www.linux-kvm.org/page/Main_Page) acceleration.

## HOWTO
Besides QT Creator makefiles, there is custom Makefile to simplify building benchmark on base system.
Use bash scripts to automate execution process. Scripts run benchmark with all files in provided data set.
1. run.sh - runs benchmark with high compression level
2. run-low.sh - runs benchmark with low compression level
Now, result file can be processed with create_stats.py script which convert results to other format - with values only.

`make
./run.sh > results.txt
python2.7 create_stats.py > stats.txt`



## Used compression libraries

1. zlib: http://www.zlib.net/
2. libbzip2: http://www.bzip.org/
3. snappy: https://code.google.com/p/snappy/
4. LZO: http://www.oberhumer.com/opensource/lzo/