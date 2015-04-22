#include "VMSystem.h"

static const int DEBUG = 0;

VMSystem::VMSystem(int memorySize, std::string algo){
	RAMSize = memorySize;
	pageFault = 0;
	totalReferenced = 0;
	//Start counting clock time
	startTime = std::chrono::high_resolution_clock::now();
	// seed RNG
	gen.seed(startTime.time_since_epoch().count());

	if(algo == "RAND"){
	  policy = RAND;
	}
	else if(algo == "LRU"){
	  policy = LRU;
	}
	else{
	  policy = FIFO;
	}
}

void VMSystem::startProcess(int processID, int memorySize){
	std::vector<bool> tmp (memorySize+1, false);
	VM.insert(std::pair<int, std::vector<bool> >(processID, tmp));
}

void VMSystem::terminateProcess(int processID){
	//remove VM
	VM.erase(processID);
	//clear reference in RAM
	RAM.erase(processID);
}

void VMSystem::referenceProcess(int processID, int pageNumber){
  if(DEBUG) std::cerr << "reference: pid " << processID << " page " << pageNumber << std::endl; 
	//Record the time for a page is referenced
	std::chrono::high_resolution_clock::time_point stopTime = std::chrono::high_resolution_clock::now();
	//Interval from program starting to a page reference
	double elapsed_time = std::chrono::duration<double, std::milli> (stopTime-startTime).count();

	totalReferenced++;
	//Page has not allocate in RAM previously
	if( (VM.find(processID) -> second)[pageNumber] == false){
          //RAM is full --- remove one page first and then perform insertion
	  if(RAM.size() >= (unsigned)RAMSize){
		//page swap algorithm here
		  switch(policy){
		  case RAND:
		    rand();
		    break;
		  case LRU:
		    lru();
		    break;
		  case FIFO:
		    fifo();
		    break;
		  default:
		    std::cerr << "unknown policy: " << policy << std::endl;
		  }
	  }
	  //If RAM is no full and the page is not allocated in RAM 
	  struct info tmp;
	  tmp.pageNumber = pageNumber;
	  tmp.arrivalTime = elapsed_time;
	  tmp.lastUsedTime = elapsed_time;	
	  RAM.insert(std::pair<int, info> (processID, tmp) );
	  (VM.find(processID) -> second)[pageNumber] = true;
	
		pageFault++;
	}
	//Page previously allocated
	else{
		(RAM.find(processID) -> second).lastUsedTime = elapsed_time;
	}
}

void VMSystem::rand(void){
  std::unordered_multimap<int, info>::iterator it, victim;
  
  // Select a random page as victim
  int offset = gen() % RAM.size();
  victim = RAM.begin();
  for(int i = 0; i < offset; i++)
    victim++;
  if(DEBUG){
    std::cerr << "in rand:\nRAM:" << std::endl;
    for(it = RAM.begin(); it != RAM.end(); it++){
      std::cerr << it->first << " " << it->second.pageNumber << std::endl;
    }
    std::cerr << "generated: " << offset << std::endl;
    std::cerr << "victim: " << victim->first << " " << victim->second.pageNumber << std::endl;
    std::cerr << "in rand: before erase, size = " << RAM.size() << std::endl;
  }
  int pid = victim->first;
  int page = victim->second.pageNumber;
  ((VM.find(pid))->second)[page] = false;
  RAM.erase(victim);
  if(DEBUG){
    std::cerr << "in rand: after erase, size = " << RAM.size() << std::endl;       
    for(it = RAM.begin(); it != RAM.end(); it++){
      std::cerr << it->first << " " << it->second.pageNumber << std::endl;
    }
  }
}

void VMSystem::lru(void){
	std::unordered_multimap<int, info>::iterator it, victom;

	double min = 9999999999;
	//Find the page that was not referenced for a long time	
	for(it = RAM.begin(); it != RAM.end(); it++){
		if( (it->second).lastUsedTime < min){
			min = (it->second).lastUsedTime;
			victom = it;
		}		
	}
	if(DEBUG){
	  std::cout << "in lru: before erase size = " << RAM.size() << " victom is " << victom->first << "--" << victom->second.pageNumber << std::endl;
	}
	int pid = victom->first;
	int page = victom->second.pageNumber;
	((VM.find(pid))->second)[page] = false;
	RAM.erase(victom);
	if(DEBUG){
	std::cout << "in lru: after erase size = " << RAM.size() << std::endl;
	}
}

void VMSystem::fifo(void){

	std::unordered_multimap<int, info>::iterator it, victom;

	double min = 9999999999;
	//Find the page that was not referenced for a long time	
	for(it = RAM.begin(); it != RAM.end(); it++){
		if( (it->second).arrivalTime < min){
			min = (it->second).arrivalTime;
			victom = it;
		}		
	}
	if(DEBUG){
	  std::cout << "in fifo: before erase size = " << RAM.size() << " victom is " << victom->first << "--" << victom->second.pageNumber << std::endl;
	}
	int pid = victom->first;
	int page = victom->second.pageNumber;
	((VM.find(pid))->second)[page] = false;
	RAM.erase(victom);
	if(DEBUG){
	std::cout << "in fifo: after erase size = " << RAM.size() << std::endl;
	}
}

void VMSystem::simulate(std::string fileName){
	std::ifstream inFile(fileName.c_str());
	if(!(inFile.good())){
		std::cerr << "Cannot open file: " << fileName << std::endl;
		exit(1);
	}
	std::string line, word;
	int processID, pageNumber, size;
	while(std::getline(inFile, line)){
		std::stringstream ss;
		ss << line;
		ss >> word;
		if(word.compare("START") == 0){
			ss >> processID >> size;
			startProcess(processID, size);
		}else if(word.compare("REFERENCE") == 0){
			ss >> processID >> pageNumber;
			referenceProcess(processID, pageNumber);
		}else if(word.compare("TERMINATE") == 0){	
			ss >> processID;
			terminateProcess(processID);
		}else{
			std::cerr << "Unknown input line from file: " << fileName << std::endl;
			exit(1);
		}
	}
	std::cout << "Page fault rate: " << pageFaultRate(pageFault, totalReferenced)*100 << "%" << std::endl;
}

double VMSystem::pageFaultRate(double pageFault, double totalReferenced){
	return pageFault/totalReferenced;
}
