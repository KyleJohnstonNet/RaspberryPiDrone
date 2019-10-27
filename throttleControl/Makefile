#
#	Copyright 2019 Kyle Johnston
#
# 	Makefile for Throttle Control
#

CXX ?= g++
FLAGS = -Wall -W -ggdb -std=c++17

NAVIO = /home/pi/Code/Navio2/C++/Navio
ESCINTERFACE = ../EscInterface
SHAREDMEMORYSTRUCTS = /home/pi/Code/sharedMemoryStructs

INCLUDES = -I $(ESCINTERFACE) -I $(NAVIO) -I $(SHAREDMEMORYSTRUCTS)
LIBRARIES = -L $(NAVIO) -lnavio -pthread

SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) $(FLAGS) $(INCLUDES) *.cpp $(LIBRARIES)  -o $@ $< 

all: $(OBJECTS)
	rm -vf throttleControl
	$(MAKE) -C $(NAVIO) all
	$(MAKE) -C $(ESCINTERFACE) all
	$(CXX) $(FLAGS) throttleControl.cpp $(ESCINTERFACE)/EscInterface.o $(INCLUDES) $(LIBRARIES) -o throttleControl

clean:
	rm -vf throttleControl
	rm -vf *.o

cleanall: clean
	# $(MAKE) -C $(NAVIO) clean
	$(MAKE) -C $(ESCINTERFACE) clean