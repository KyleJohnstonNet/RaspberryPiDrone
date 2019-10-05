#
#	Copyright 2019 Kyle Johnston
#
# 	Makefile for Read Sensors
#

CXX ?= g++
FLAGS = -Wall -W -ggdb
NAVIO = /home/pi/Code/Navio2/C++/Navio
SHAREDMEMORYSTRUCTS = /home/pi/Code/sharedMemoryStructs
INCLUDES = -I $(NAVIO) -I $(SHAREDMEMORYSTRUCTS)
LIBRARIES = -L $(NAVIO)

SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) -std=c++17 $(INCLUDES) *.cpp  -o $@ $< 

all: $(OBJECTS)
	rm -vf readSensors
	$(MAKE) -C $(NAVIO) all
	$(CXX) $(FLAGS) readSensors.cpp $(INCLUDES) $(LIBRARIES) -lnavio -o readSensors

clean:
	rm -vf readSensors
	rm -vf *.o

cleanall: clean
	# $(MAKE) -C $(NAVIO) clean