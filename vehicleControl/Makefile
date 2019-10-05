#
#	Copyright 2019 Kyle Johnston
#
# 	Makefile for Vehicle Control
#

CXX ?= g++
FLAGS = -Wall -W -ggdb
# NAVIO = /home/pi/Code/Navio2/C++/Navio
SHAREDMEMORYSTRUCTS = /home/pi/Code/sharedMemoryStructs
INCLUDES = -I $(SHAREDMEMORYSTRUCTS)
LIBRARIES = 

SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) -std=c++17 $(INCLUDES) *.cpp  -o $@ $< 

all: $(OBJECTS)
	rm -vf vehicleControl
	# $(MAKE) -C $(NAVIO) all
	$(CXX) $(FLAGS) vehicleControl.cpp $(INCLUDES) $(LIBRARIES) -o vehicleControl

clean:
	rm -vf vehicleControl
	rm -vf *.o

cleanall: clean
	# $(MAKE) -C $(NAVIO) clean
	$(MAKE) -C $(THROTTLECONTROL) clean