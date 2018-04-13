
DEFS = -g -std=c++11 -Wall
LIBS = -lboost_system

all: main.o
	g++ -g main.o -o main $(LIBS)


main.o: main.cpp
	g++ $(DEFS) main.cpp -c -o main.o 

server.o: server.cpp
	g++ $(DEFS) server.cpp -c -o server.o 

clean:
	rm -f *.o
	rm -f server
	
