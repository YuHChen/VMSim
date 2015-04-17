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

class VMSystem{

  public:
	VMSystem(int memorySize);
	
	void simulate(std::string fileName);
	
	void startProcess(int processID, int memorySize);

	void terminateProcess(int processID);

	void referenceProcess(int processID, int pageNumber);

	double pageFaultRate(double pageFault, double totalReference);  

  private:
	//Physical memory specifies which page slots allocated to each process
	//<processID, pageNumber>
	std::multimap<int, int> RAM;
	//Page table for each live process
	std::map<int, std::vector<bool> > VM;
	
	double pageFault;
	double totalReferenced;
	long long RAMSize;
};

#endif
