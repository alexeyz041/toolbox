CXX = g++
LINK = g++

INCLUDES =
CXXFLAGS = -c -g -std=c++11 $(INCLUDES)
LFLAGS = -lSDL2 -lSDL2_image -lpthread

all: main.o serversocket.o screen.o depacketizer.o
	$(LINK) main.o serversocket.o screen.o depacketizer.o -o udp-viewer $(LFLAGS) 

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main.o

screen.o: screen.cpp
	$(CXX) $(CXXFLAGS) screen.cpp -o screen.o

serversocket.o: serversocket.cpp
	$(CXX) $(CXXFLAGS) serversocket.cpp -o serversocket.o

depacketizer.o: depacketizer.cpp
	$(CXX) $(CXXFLAGS) depacketizer.cpp -o depacketizer.o

clean:
	rm -f udp-viewer
	rm -f *.o



