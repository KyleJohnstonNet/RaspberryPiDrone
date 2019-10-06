CXX ?= g++
NAVIO = /home/pi/Code/Navio2/C++/Navio
INCLUDES = -I $(NAVIO) -I ../../*

SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) -std=c++17 $(INCLUDES) *.cpp -L$(NAVIO) -lnavio -o $@ $< 

all: $(OBJECTS)
	$(MAKE) -C $(NAVIO) all
	$(CXX) -std=c++17 *.cpp  $(INCLUDES) -L$(NAVIO) -lnavio -lpthread 
	$(CXX) -std=c++17 BlheliProgrammer.cpp $(INCLUDES) -L$(NAVIO) -lnavio -o BlheliProgrammer

clean:
	rm -f BlheliProgrammer
	rm -f *.o
	rm -f *.out
