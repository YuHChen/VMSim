#include "VMSystem.h"

void usage(char *progName){
  std::cerr << "usage: " << progName <<" <frames-of-memory> <input-file> [RAND|LRU|FIFO]" << std::endl;  
}

int main(int argc, char **argv){
	if(argc != 4){
	  usage(argv[0]);
	  exit(1);
	}
	
	std::string algo(argv[3]);
	if(!( (algo == "RAND") || (algo == "LRU") || (algo == "FIFO") )){
	  usage(argv[0]);
	  exit(1);
	}

	VMSystem VM(atoi(argv[1]), algo);
	VM.simulate(argv[2]);
}
