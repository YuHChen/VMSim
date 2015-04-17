#include "VMSystem.h"

VMSystem::VMSystem(int memorySize){
	RAMSize = memorySize;
}

void VMSystem::startProcess(int processID, int memorySize){
	std::vector<bool> tmp (memorySize, false);
	VM.insert(std::pair<int, std::vector<bool> >(processID, tmp));
}

void VMSystem::terminateProcess(int processID){
	//remove VM
	VM.erase(processID);
	//clear reference in RAM
	RAM.erase(processID);
}

void VMSystem::referenceProcess(int processID, int pageNumber){
	totalReferenced++;
	//Page has not allocate in RAM previously
	if( (VM.find(processID) -> second)[pageNumber] == false){
		//If RAM is no full and the page is not allocated in RAM 
		if(RAM.size() < RAMSize){
			RAM.insert(std::pair<int, int> (processID, pageNumber) );
			(VM.find(processID) -> second)[pageNumber] = true;
		}
		//RAM is full
		else{
			//page swap algorithm here
		}
		pageFault++;
	}
	//Page previously allocated
	else{
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
