#ifndef _VMSYSTEM_H
#define _VMSYSTEM_H

#include<iostream>
#include<unordered_map>
#include<map>
#include<string>
#include<vector>
#include<fstream>
#include<stdlib.h>
#include<sstream>
#include<chrono>
#include <random>

struct info{
	double arrivalTime;
	double lastUsedTime;
	int pageNumber;
};

enum Policy { RAND, LRU, FIFO };

class VMSystem{

 public:
  VMSystem(int memorySize, std::string algo);
	
	void simulate(std::string fileName);
	
  private:
	//Physical memory specifies which page slots allocated to each process
	//<processID, pageNumber>
	std::unordered_multimap<int, info> RAM;
	//Page table for each live process
	std::map<int, std::vector<bool> > VM;
	
	double pageFault;
	double totalReferenced;
	long long RAMSize;
	Policy policy;
	
	std::chrono::high_resolution_clock::time_point startTime;

	std::minstd_rand gen;

	void startProcess(int processID, int memorySize);

	void terminateProcess(int processID);

	void referenceProcess(int processID, int pageNumber);

	double pageFaultRate(double pageFault, double totalReference);  
	
	void updateTime(int processID, int pageNumber, double time);
	// replacement algorithms
	void rand(void);
	void lru(void);
	void fifo(void);
};

#endif
