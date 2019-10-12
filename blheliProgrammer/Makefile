CXX ?= g++

PROJECT      = /home/pi/RaspberryPiDrone
NAVIO        = $(PROJECT)/Navio2/C++/Navio
ESCINTERFACE = $(PROJECT)/EscInterface/

INCLUDES = -I $(NAVIO) -I $(ESCINTERFACE)
LINKER = -L $(NAVIO) -L $(ESCINTERFACE) -lnavio -lpthread


SRC=$(wildcard */*.cpp)
OBJECTS = $(SRC:.cpp=.o) 

%.o: %.cpp
	$(CXX) -std=c++17 *.cpp $(INCLUDES)  $(LINKER) -lnavio -o $@ $< 

all: $(OBJECTS)
	$(MAKE) -C $(NAVIO) all
	$(MAKE) -C $(ESCINTERFACE) all
	# Hard coded to object file because I don't know how the linker works
	$(CXX) -std=c++17 BlheliProgrammer.cpp $(INCLUDES) $(LINKER) ../EscInterface/EscInterface.o -o BlheliProgrammer

clean:
	rm -f BlheliProgrammer
	rm -f *.o
	rm -f *.out

cleanall: clean
	$(MAKE) -C $(ESCINTERFACE) clean
