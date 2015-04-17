GCC = g++
FLAGS = -g -Wall
COMPILER = -std=c++0x
EXE = VMSystem

all: Driver.o VMSystem.o
	$(GCC) $(FLAGS) $(COMPILER) -o $(EXE) Driver.o VMSystem.o
Driver.o: Driver.cpp
	$(GCC) $(FLAGS) $(COMPILER) -c Driver.cpp
VMSystem.o: VMSystem.cpp
	$(GCC) $(FLAGS) $(COMPILER) -c VMSystem.cpp
clean:
	rm -f *.o $(EXE)
