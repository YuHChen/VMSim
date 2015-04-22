EXE = VMSystem
GEN = gen

SRC = \
	Driver.cpp \
	VMSystem.cpp \

OBJ_FILES := $(SRC:.cpp=.o)

GEN_SRC = Generator.cpp
GEN_OBJ := $(GEN_SRC:.cpp=.o)

GCC = g++
FLAGS = -g -Wall
COMPILER = -std=c++11

all:	$(EXE)

$(EXE):	$(OBJ_FILES)
	$(GCC) $(FLAGS) $(COMPILER) $(OBJ_FILES) -o $@

# common build recipe for .o
%.o:	%.cpp
	$(GCC) $(FLAGS) $(COMPILER) -c $^

# build input file generator
$(GEN):	$(GEN_OBJ)
	$(GCC) $(FLAGS) $(COMPILER) $^ -o $@

clean:
	rm -f *.o $(EXE) $(GEN)
