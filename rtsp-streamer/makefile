CXX = g++
LINK = g++

INCLUDES = -I../live/UsageEnvironment/include -I../live/groupsock/include -I../live/liveMedia/include -I../live/BasicUsageEnvironment/include -I../x264
INCLUDES += -I/usr/X11R6/include 
CXXFLAGS = -c -g -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 $(INCLUDES) -std=c++11 -D_GNU_SOURCE
#CXXFLAGS += -DUSE_X264

LFLAGS = -L../live/BasicUsageEnvironment -L../live/UsageEnvironment -L../live/liveMedia -L../live/groupsock -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment
LFLAGS += -L/usr/X11R6/lib -lX11 -lXext
#LFLAGS += ../x264/libx264.a
LFLAGS += -lm -lpthread -ldl -lavformat -lavcodec -lswresample -lswscale -lavutil -lavfilter -lv4l2 

all: streamer.o source.o screenshot.o scaler.o encoder2.o crop.o camera.o mediasubsession.o 
	$(LINK) streamer.o source.o screenshot.o scaler.o encoder2.o crop.o camera.o mediasubsession.o -o rtsp-streamer $(LFLAGS) 

streamer.o: streamer.cpp
	$(CXX) $(CXXFLAGS) streamer.cpp -o streamer.o

source.o: source.cpp
	$(CXX) $(CXXFLAGS) source.cpp -o source.o

#encoder.o: encoder.cpp encoder.h 
#	$(CXX) $(CXXFLAGS) -c encoder.cpp -o encoder.o

encoder2.o: encoder2.cpp encoder2.h 
	$(CXX) $(CXXFLAGS) -c encoder2.cpp -o encoder2.o

crop.o: crop.cpp crop.h 
	$(CXX) $(CXXFLAGS) -c crop.cpp -o crop.o

screenshot.o: screenshot.cpp screenshot.h
	$(CXX) $(CXXFLAGS) screenshot.cpp -o screenshot.o

scaler.o: scaler.cpp scaler.h
	$(CXX) $(CXXFLAGS) scaler.cpp -o scaler.o

camera.o: camera.cpp camera.h
	$(CXX) $(CXXFLAGS) camera.cpp -o camera.o

mediasubsession.o: mediasubsession.cpp mediasubsession.h
	$(CXX) $(CXXFLAGS) mediasubsession.cpp -o mediasubsession.o

clean:
	rm -f rtsp-streamer
	rm -f *.o



