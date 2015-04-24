#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

#include <getopt.h>
#include <ctype.h>

// ranges and default values
#define DEF_P 5
#define LO_P 1
#define HI_P 100
#define DEF_F 5
#define LO_F 1
#define HI_F 100
#define LO_M 1
#define HI_M 20000
#define DEF_D 5
#define LO_D 1
#define HI_D 10
#define DEF_OUT "in.txt"

// limits
#define BURST_LIMIT 10 // max num of references a process can make consecutively

#define DEBUG 0 // 1 to turn debug on

void usage(char *progName){
  std::cerr << "Usage: " << progName << " [-u usage] [-p <num-processes>] [-m <memory-needed>] [-f <num-frames>] [-d <deg-of-locality>] [-o <output-file>] [-P phases-mode] [-H high-mem-pressure] [-h help]" << std::endl;
  std::cerr << "Try '" << progName << " -h' for more information." << std::endl;
}

void help(char *progName){
  std::cerr << "Usage: " << progName << " [-u usage] [-p <num-processes>] [-m <memory-needed>] [-f <num-frames>] [-d <deg-of-locality>] [-P phases-mode] [-H high-mem-pressure] [-h help]" << std::endl;
  std::cerr << "  -p: number of processes to simulate, range: ["<<LO_P<<","<<HI_P<<"], default: " << DEF_P << std::endl;
  std::cerr << "  -f: number of frames in RAM, range: ["<<LO_F<<","<<HI_F<<"], default: " << DEF_F << std::endl;
  std::cerr << "  -m: total amount of memory required by all processes, range: ["<<LO_M<<","<<HI_M<<"], default: <num-processes>" << std::endl;
  std::cerr << "  -d: degree of locality of reference processes exhibit, range: ["<<LO_D<<","<<HI_D<<"], default: " << DEF_D << std::endl;
  std::cerr << "  -o: name of output file" << std::endl;
  std::cerr << "  -P: toggle phases mode, in which references are generated in phases" << std::endl;
  std::cerr << "  -H: toggle high memory pressure mode, in which <memory-needed> upper limit is removed" << std::endl;
}

int extractR(char *prog, std::string name, std::string in, int min, int max, int def){
  if(in.empty()){
    return def;
  }
  std::stringstream buf;
  int rv;
  buf << in;
  buf >> rv;
  if( (rv >= min) && (rv <= max) )
    return rv;
  std::cerr << prog << ": invalid value for <" << name << ">, range: [" << min << "," << max << "], but received " << rv << std::endl;
  exit(EXIT_FAILURE);
  return -1;
}

std::string start(int processID, int capacity){
  std::stringstream ss;
  ss << "START " << processID << " " << capacity;
  std::string rv;
  std::getline(ss, rv);
  return rv;
}

std::string term(int processID){
  std::stringstream ss;
  ss << "TERMINATE " << processID;
  std::string rv;
  std::getline(ss, rv);
  return rv;  
}

std::string ref(int processID, int pageNumber){
  std::stringstream ss;
  ss << "REFERENCE " << processID << " " << pageNumber;
  std::string rv;
  std::getline(ss, rv);
  return rv;
}

int main(int argc, char **argv){
  if(argc == 1){
	usage(argv[0]);
	return 1;
  }
  /*** flags and values ***/
  std::string pval, mval, fval, dval, oval;
  bool Pflag = false;
  bool Hflag = false;
  int c;

  /*** externs for getopt ***/
  extern char *optarg;
  extern int optopt, opterr;
  opterr = 0; // (default) nonzero = getopt prints error message

  /*** parse command line arguments ***/
  while( (c = getopt(argc, argv, "up:m:f:d:o:PHh")) != -1 ){
    switch (c){
    case 'u':
      usage(argv[0]);
      return 1;
    case 'p':
      pval = optarg;
      break;
    case 'm':
      mval = optarg;
      break;
    case 'f':
      fval = optarg;
      break;
    case 'd':
      dval = optarg;
      break;
    case 'o':
      oval = optarg;
      break;
    case 'P':
      Pflag = true;
      break;
    case 'H':
      Hflag = true;
      break;
    case 'h':
      help(argv[0]);
      return 1;
    case '?':
      if( (optopt == 'p') || (optopt == 'm') || (optopt == 'f') ||
	  (optopt == 'd') || (optopt == 'o') ){
	// user did not provide required argument
	std::cerr << argv[0] << ": option -" << (char)optopt << " requires an argument" << std::endl;
	usage(argv[0]);
	exit(EXIT_FAILURE);
      }
      else if(isprint(optopt)){
	// user specified unsupported option
	std::cerr << argv[0] << ": invalid option -" << (char)optopt << std::endl;
	usage(argv[0]);
	exit(EXIT_FAILURE);
      }
      break;
    default:
      std::cerr << argv[0] << ": invalid option -" << (char)optopt << std::endl;
      usage(argv[0]);
      exit(EXIT_FAILURE);
      break;
    }
  }
  /*** end of parsing command line ***/
 
  if(DEBUG){
    std::cerr << "completed parsing command line" << std::endl;
    std::cerr << "pval="<<pval<<", mval="<<mval<<", fval="<<fval<<", dval="<<dval<<", oval="<<oval<<", Pflag="<<Pflag<<", Hflag="<<Hflag << std::endl;
  }

  /*** extract and set parameters ***/
  int maxProcesses = extractR(argv[0], "num-processes", pval, LO_P, HI_P, DEF_P); 
  int maxFrames = extractR(argv[0], "num-frames", fval, LO_F, HI_F, DEF_F); 
  int degree = extractR(argv[0], "deg-of-locality", dval, LO_D, HI_D, DEF_D); 
  int maxVMemory;
  if(Hflag){
    std::stringstream buf;
    buf << mval;
    buf >> maxVMemory;
    if(maxVMemory < LO_M){
      std::cerr << argv[0] << ": invalid value for <memory-needed>, range: [" << LO_M << ",INF], but received " << maxVMemory << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else{
    maxVMemory = extractR(argv[0], "memory-needed", mval, LO_M, HI_M, maxProcesses); 
  }  
  if(oval.empty()){
    oval = DEF_OUT;
  }

  if(DEBUG){
    std::cerr << "completed extracting and setting parameters" << std::endl;
    std::cerr << "maxProcesses="<<maxProcesses<<", maxVMemory="<<maxVMemory<<", maxFrames="<<maxFrames<<", degree="<<degree<<", oval="<<oval<<", Pflag="<<Pflag<<", Hflag="<<Hflag << std::endl;
  }

  /*** generate instructions ***/
  if(DEBUG) std::cerr << argv[0] << ": generating..." << std::endl;

  std::vector<int> pCaps(maxProcesses+1,0);
  std::vector<int> prevRef(maxProcesses+1,0);
  std::vector<int> bursts(maxProcesses+1,0);
  std::vector<int> alive;
  std::vector<int> dead;
  int pid, cap, page;
  int nProcesses = 0;
  int nVMemory = 0;
  //int nFrames = 0;
  int threshold = 0;
  int bound = maxVMemory;
  std::ofstream outfile;
  outfile.open(oval);

  if(Pflag) bound += maxProcesses * BURST_LIMIT;
  if(Hflag) bound = (2*HI_M) + bound;

  // seed rng
  std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
  std::minstd_rand rng(time.time_since_epoch().count());

  // first instruction: start up the first process
  pid = (rng() % maxProcesses)+1;
  cap = 1 + (rng() % (maxVMemory - maxProcesses));
  outfile << start(pid, cap) << std::endl;
  alive.push_back(pid);
  pCaps[pid] = cap;
  nProcesses++;
  nVMemory += cap;

  // randomly generate remaining instructions
  do{
    // START
    int chance = rng()%10;
    if( (chance < 2) && (nProcesses < maxProcesses) ){
      // 20% chance to start a process
      if(DEBUG) std::cerr << "STA" << std::endl;
      // select process to start up
      do{
	pid = (rng() % maxProcesses) + 1;
      } while( (std::count(alive.begin(), alive.end(), pid) > 0) ||
	       (std::count(dead.begin(), dead.begin(), pid) > 0) );
      if(DEBUG) std::cerr << "calc cap: n=" << nVMemory << " %=" << maxVMemory - nVMemory + 1 << std::endl;
      // determine virtual memory capacity of this process
      int memLeft = maxVMemory - nVMemory - maxProcesses + nProcesses;
      if(memLeft > 0)
	cap = 1 + (rng() % memLeft);
      else
	cap = 1;
      if(DEBUG) std::cerr << "cap=" << cap << std::endl;
      // start process and update values
      outfile << start(pid, cap) << std::endl;
      alive.push_back(pid);
      pCaps[pid] = cap;
      nProcesses++;
      nVMemory += cap;
    }
    // TERMINATE
    else if( ((chance*2) < 5) && (nProcesses >= maxProcesses) && (threshold > bound/4) && !alive.empty() ){
      // 5% chance to terminate a process, but
      // start terminating processes only if all of them has had a chance to start up
      // and if threshold reaches 25% of bound
      //!!!should change selection of process to terminate to show effects of different algorithms?
      if(DEBUG) std::cerr << "TERM" << std::endl;
      // select process to terminate
      std::random_shuffle(alive.begin(), alive.end());
      pid = alive.back();
      // terminate process and update values;
      outfile << term(pid) << std::endl;
      alive.pop_back();
      dead.push_back(pid);
    }
    // REFERENCE
    else if( (chance >= 3) && !alive.empty() ){
      // 75% chance to make a memory reference
      //!!!should change selection of reference to show effects of different algorithms?
      if(DEBUG) std::cerr << "REF" << std::endl;
      
      // select a process to make references
      std::random_shuffle(alive.begin(), alive.end());
      pid = alive.back();
      
      // select a page
      if(Pflag){
	// phases mode (processes make references in phases, i.e. a burst of references to the same page)
	// reference prev page
	page = prevRef[pid];
	if(page <= 0){
	  // first memory reference, choose random page 
	  page = (rng() % pCaps[pid]) + 1;
	  bursts[pid] = 1 + (rng() % BURST_LIMIT);
	}
	else if(bursts[pid] <= 0){
	  // previous phase completed, moving to next phase
	  //!!!should phases mode exhibit spatial locality??? (limit how new page is selected)
	  do{
	    page = (rng() % pCaps[pid]) + 1;
	    //if(pCaps[pid] == 1) break; // if virtual memory size of process pid is 1 then there is no new page
	  } while( (page == prevRef[pid]) && (pCaps[pid] > 1) ); 
	  bursts[pid] = 1 + (rng() % BURST_LIMIT);
	}
      }
      else{
	// determine whether to reference new page or old page (temporal locality)
	if( (rng() % 10) < (unsigned)(11-degree) ){
	  // (10*(11-degree)% chance to reference new page
	  do{
	    page = (rng() % pCaps[pid]) + 1;
	    //if(pCaps[pid] == 1) break; // if virtual memory size of process pid is 1 then there is no new page
	  } while( (page == prevRef[pid]) && (pCaps[pid] > 1) );
	}
	else{
	  // reference prev page
	  page = prevRef[pid];
	  if(page <= 0)
	    page = (rng() % pCaps[pid]) + 1;
	}
	
      }

      // make reference and update values
      outfile << ref(pid, page) << std::endl;
      threshold++;
      prevRef[pid] = page;
      if(Pflag) bursts[pid] -= 1;
    }
    
  } while( (threshold < bound) && !alive.empty() );  // stop if there are no processes left or bound references has been made
  
  if(DEBUG) std::cerr << "threshold: " << threshold << " bound: " << bound << std::endl;

  /*** clean up resources ***/
  outfile.close();
  
  if(DEBUG) std::cerr << argv[0] << ": done generating" << std::endl;
  return 0;
}
