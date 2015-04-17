TARGET = VMSim

SRC = \
	main.cpp

CPPFLAGS = -g -Wall -std=c++0x

OBJ_FILES := $(SRC:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJ_FILES)
	g++ $(CPPFLAGS) $(OBJ_FILES) -o $@.exe

%.o:	%.cpp
	g++ $(CPPFLAGS) -c $^ -o $@

clean:
	rm *~ *# *.o *.exe $(TARGET)