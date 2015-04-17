#include "VMSystem.h"
int main(int argc, char **argv){
	if(argc != 3){
		std::cerr << "./VMSystem <frames-of-memory> <input-file>" << std::endl;
		exit(1);
	}
	VMSystem VM(atoi(argv[1]));
	VM.simulate(argv[2]);
}
