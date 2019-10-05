#
#	Copyright 2019 Kyle Johnston
#
# 	Makefile for Throttle Control
#

CXX ?= g++
FLAGS = -Wall -W -ggdb
NAVIO = /home/pi/Code/Navio2/C++/Navio
ESCINTERFACE = /home/pi/Code/EscInterface
SHAREDMEMORYSTRUCTS = /home/pi/Code/sharedMemoryStructs
INCLUDES = -I $(ESCINTERFACE) -I $(NAVIO) -I $(SHAREDMEMORYSTRUCTS)
LIBRARIES = -L $(NAVIO)

SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) -std=c++17 $(INCLUDES) *.cpp $(LIBRARIES) -lnavio -o $@ $< 

all: $(OBJECTS)
	rm -vf throttleControl
	$(MAKE) -C $(NAVIO) all
	$(MAKE) -C $(ESCINTERFACE) all
	# $(CXX) $(FLAGS) -c $(INCLUDES) main.cpp
	# $(CXX) $(FLAGS) $(INCLUDES) *.o $(LIBRARIES) -lnavio -o throttleControl
	$(CXX) $(FLAGS) throttleControl.cpp $(ESCINTERFACE)/*.o $(INCLUDES) $(LIBRARIES) -lnavio -o throttleControl

clean:
	rm -vf updateThrottle
	rm -vf *.o

cleanall: clean
	# $(MAKE) -C $(NAVIO) clean
	$(MAKE) -C $(ESCINTERFACE) clean