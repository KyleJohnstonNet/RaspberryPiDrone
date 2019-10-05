CXX ?= g++
NAVIO = /home/pi/Code/Navio2/C++/Navio
INCLUDES = -I /home/pi/Code/Navio2/C++/Navio

SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) -std=gnu++11 $(INCLUDES) *.cpp -L$(NAVIO) -lnavio -o $@ $< 

all: $(OBJECTS)
	$(MAKE) -C /home/pi/Code/Navio2/C++/Navio all
	$(CXX) -std=gnu++11 $(INCLUDES) *.cpp -L$(NAVIO) -lnavio -lpthread -o $@ $< 
	$(CXX) -std=gnu++11 $(INCLUDES) BlheliProgrammer.cpp -L$(NAVIO) -lnavio -o BlheliProgrammer

clean:
	rm -f BlheliProgrammer
	rm -f *.o
