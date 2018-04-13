
DEFS = -g -std=c++11
LIBS = -lboost_system

all: main.o
	g++ -g main.o -o main $(LIBS)


main.o: main.cpp
	g++ $(DEFS) main.cpp -c -o main.o 

clean:
	rm -f *.o
