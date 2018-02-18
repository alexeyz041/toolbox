CXX = g++
LINK = g++

INCLUDES = 
CXXFLAGS = -c -g -std=c++11 $(INCLUDES)

LFLAGS = -lavfilter -lavformat -lavcodec -lswscale -lavutil
LFLAGS += -L/usr/X11R6/lib -lX11 -lXext

all: main.o packetizer.o screenshot.o png.o crop.o
	$(LINK) main.o packetizer.o screenshot.o png.o crop.o -o udp-streamer $(LFLAGS) 

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main.o

packetizer.o: packetizer.cpp
	$(CXX) $(CXXFLAGS) packetizer.cpp -o packetizer.o

screenshot.o: screenshot.cpp
	$(CXX) $(CXXFLAGS) screenshot.cpp -o screenshot.o

png.o: png.cpp
	$(CXX) $(CXXFLAGS) png.cpp -o png.o

jpeg.o: jpeg.cpp
	$(CXX) $(CXXFLAGS) jpeg.cpp -o jpeg.o

crop.o: crop.cpp
	$(CXX) $(CXXFLAGS) crop.cpp -o crop.o

clean:
	rm -f udp-streamer
	rm -f *.o



