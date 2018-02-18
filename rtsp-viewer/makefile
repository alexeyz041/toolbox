CXX = g++
LINK = g++

INCLUDES = -I../live/UsageEnvironment/include -I../live/groupsock/include -I../live/liveMedia/include -I../live/BasicUsageEnvironment/include
CXXFLAGS = -c -g -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE $(INCLUDES) -std=c++11 

LFLAGS = -L../live/BasicUsageEnvironment -L../live/UsageEnvironment -L../live/liveMedia -L../live/groupsock -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment
LFLAGS += -lavfilter -lavformat -lavcodec -lswresample -lswscale -lavutil -lSDL2 -lSDL2_image
LFLAGS += -lm -lpthread -ldl -lz

all: main.o decoder.o screen.o
	$(LINK) main.o decoder.o screen.o -o rtsp-viewer $(LFLAGS) 

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main.o

decoder.o: decoder.cpp
	$(CXX) $(CXXFLAGS) decoder.cpp -o decoder.o

screen.o: screen.cpp
	$(CXX) $(CXXFLAGS) screen.cpp -o screen.o

clean:
	rm -f rtsp-viewer
	rm -f *.o



