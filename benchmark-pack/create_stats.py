import sys, getopt

def main(progname, argv):
  
  inputfilename = "results.txt"
  
  #Command line arguments
  try:
	opts, args = getopt.getopt(argv, "i:")
  except getopt.GetoptError:
	print "Usage: " + progname + " [-i inputfile]"
	sys.exit(2)
	
  for opt, arg in opts:
	if opt == "-i":
	  inputfilename = arg
	  
  print "Input file name set to: " + inputfilename
  
  #inputfile = open(inputfilename, 'r')
  with open(inputfilename, 'r') as inputfile:
	for line in inputfile:
		if "* LZO *" in line:
		  print line
		elif "* BZIP2 *" in line:
		  print line
		elif "* ZLIB *" in line:
		  print line
		elif "* SNAPPY *" in line:
		  print line
		elif "file:" in line:
		  line = line.split("/")
		  print line[2][:len(line[2])-1]
		elif "Mean compression time:" in line:
		  line = line.split(" ")
		  print line[3].replace('.',',')
		elif "Mean compression ratio:" in line:		
		  line = line.split(" ")
		  print line[3][:len(line[3])-1].replace('.',',')
		elif "Mean decompression time:" in line:
		  line = line.split(" ")
		  print line[3].replace('.',','), "\n"
  inputfile.close()
  
if __name__ == "__main__":
  main(sys.argv[0], sys.argv[1:])