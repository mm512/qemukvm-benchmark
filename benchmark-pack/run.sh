#!/bin/bash
readonly ITERATIONS=10

recurse() {
	for i in "$1"/*
	do
		# if directory
		if [ -d "$i" ]; then
			#echo "dir: $i"
			recurse "$i" "$2" "$3"
		# if file
		elif [ -f "$i" ]; then
			if [[ "$i" != *.lzo ]] && [[ "$i" != *.bz2 ]] && [[ "$i" != *.zlib ]] && [[ "$i" != *.snappy ]]; then
				echo "file: $i"
				if [ "$2" == "./qemukvm-benchmark" ]; then
				  if [ "$3" == "zlib" ]; then
						"$2" -h -t $ITERATIONS --zlib "$i"
				  elif [ "$3" == "bzip2" ]; then
						"$2" -h -t $ITERATIONS --bzip2 "$i"
				  elif [ "$3" == "snappy" ]; then
						"$2" -h -t $ITERATIONS --snappy "$i"
				  elif [ "$3" == "lzo" ]; then
						"$2" -h -t $ITERATIONS --lzo "$i"
				  fi
				fi
				printf "\n********************************\n\n"
			fi
		fi
	done
}

printf "\n\n*************** *************** ZLIB *************** ***************\n\n"
recurse testdata/text ./qemukvm-benchmark zlib
printf "\n\n*************** *************** BZIP2 *************** ***************\n\n"
recurse testdata/text ./qemukvm-benchmark bzip2
printf "\n\n*************** *************** SNAPPY *************** ***************\n\n"
recurse testdata/text ./qemukvm-benchmark snappy
printf "\n\n*************** *************** LZO *************** ***************\n\n"
#recurse testdata ./qemukvm-benchmark lzo