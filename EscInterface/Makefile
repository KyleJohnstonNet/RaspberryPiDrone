CXX ?= g++
NAVIO = /home/pi/Code/Navio2/C++/Navio
INCLUDES = -I $(NAVIO)

SRC=$(wildcard */*.cpp)

all:
	$(MAKE) -C $(NAVIO) all
	$(CXX) -c -ggdb $(INCLUDES) EscInterface.cpp -L$(NAVIO) -lnavio

clean:
	rm -vf *.o
