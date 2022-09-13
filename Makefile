#pgl header files
INCLUDEDIR= $(HOME)/PGL/include

#boost header files
BOOSTINCLUDEDIR='/usr/include/boost'

#boost bin files
BOOSTLIBDIR='/usr/lib'

all: compile

compile:
	g++ -std=c++17 main.cpp -m64 -fopenmp -o main -O3 -I$(INCLUDEDIR) -I$(BOOSTINCLUDEDIR) -L$(BOOSTLIBDIR) -lboost_program_options -Wno-unused-local-typedefs -DNDEBUG -funroll-loops -w #-ffast-math -ftree-vectorize




debug:
	g++ main.cpp -o main -O0 -g -I$(INCLUDEDIR) -I$(BOOSTINCLUDEDIR) -L$(BOOSTLIBDIR) -lboost_program_options -Wall -DMEMSTATS -funroll-loops

ex:
	g++ example.cpp -o ex -O3 -I$(INCLUDEDIR) -I$(BOOSTINCLUDEDIR) -L$(BOOSTLIBDIR) -lboost_program_options -DNDEBUG -Wall

clean:
	rm -f *~ main *.o
