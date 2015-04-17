#ifndef _VMSYSTEM_H
#define _VMSYSTEM_H

#include<iostream>
#include<map>
#include<string>
#include<vector>
#include<fstream>
#include<stdlib.h>
#include<sstream>
/*
struct info{
	double arrivalTime;
	double lastUsedTime;
	int pageNumber;
};
*/

enum Policy { RAND, LRU, FIFO };

class VMSystem{

 public:
  VMSystem(int memorySize, std::string algo);
	
	void simulate(std::string fileName);
	
  private:
	//Physical memory specifies which page slots allocated to each process
	//<processID, pageNumber>
	std::multimap<int, int> RAM;
	//Page table for each live process
	std::map<int, std::vector<bool> > VM;
	
	double pageFault;
	double totalReferenced;
	long long RAMSize;
	Policy policy;

	void startProcess(int processID, int memorySize);

	void terminateProcess(int processID);

	void referenceProcess(int processID, int pageNumber);

	double pageFaultRate(double pageFault, double totalReference);  
	// replacement algorithms
	// void rand();
	// void lru();
	// void fifo();
};

#endif
