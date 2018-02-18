
#ifndef _SOURCE_H
#define _SOURCE_H

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <deque>

#ifdef USE_X264
#include "encoder.h"
#else
#include "encoder2.h"
#endif

class SourceParameters {
public:
	SourceParameters(int w,int h,int fps) : width(w), height(h), fps(fps) {}

	int width;
	int height;
	int fps;
};


class Buffer {
public:
	Buffer(Buffer &b) : Buffer(b.buf,b.len,b.ts)
	{
	}

	Buffer(const Buffer &b) : Buffer(b.buf,b.len,b.ts)
	{
	}


	Buffer(uint8_t *b,int len,uint64_t ts) : len(len),ts(ts)
	{
		buf = (uint8_t *)malloc(len);
		if(buf) {
			memcpy(buf,b,len);
		}
	}

	~Buffer()
	{
		if(buf) {
			free(buf);
			buf = nullptr;
		}
	}

public:
	uint8_t *buf;
	int len;
	uint64_t ts;
};


#ifdef USE_X264
class Source: public FramedSource, public Encoder {
#else
class Source: public FramedSource, public Encoder2 {
#endif

public:
  static Source* createNew(UsageEnvironment& env, SourceParameters params);

public:
  static EventTriggerId eventTriggerId;
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.
protected:
  Source(UsageEnvironment& env, SourceParameters params);
  // called only by createNew(), or by subclass constructors
  virtual ~Source();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  //virtual void doStopGettingFrames(); // optional

  virtual void write(uint8_t *buf,int len,uint64_t ts);

private:
  static void deliverFrame0(void* clientData);
  void deliverFrame();

private:
  static unsigned referenceCount; // used to count how many instances of this class currently exist
  SourceParameters fParams;
  std::deque<Buffer> fifo;
  bool bNeedEvent;
  struct timeval ptime;
};

#endif //_SOURCE_H
