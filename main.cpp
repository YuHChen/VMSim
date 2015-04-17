#include <iostream>
#include <string>

void usage(char *progName){
  std::cerr << "usage: " << progName << " <frames-of-memory> <input-file> [RAND|LRU|FIFO]\n";
}

int main(int argc, char **argv){
  char *progName = argv[0];

  if(argc < 4){
    usage(progName);
    exit(EXIT_FAILURE);
  }

  std::string algo_ind(argv[3]);

  std::cout << "running algorithm: ";
  if(algo_ind == "RAND"){
    std::cout << "RANDOM\n";
  }
  else if(algo_ind == "LRU"){
    std::cout << "LRU\n";
  }
  else if(algo_ind == "FIFO"){
    std::cout << "FIFO\n";
  }
  else{
    std::cerr << "\n" << progName << ": " << algo_ind << " is not a recognized algorithm\n";
    exit(EXIT_FAILURE);
  }

  return 0;
}
