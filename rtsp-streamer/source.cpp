
#include "source.h"
#include <GroupsockHelper.hh> // for "gettimeofday()"

extern Source *source;

Source* Source::createNew(UsageEnvironment& env,SourceParameters params)
{
	if(!source) {
		source = new Source(env, params);
	}
	return source;
}


EventTriggerId Source::eventTriggerId = 0;
unsigned Source::referenceCount = 0;



Source::Source(UsageEnvironment& env,  SourceParameters params) : FramedSource(env), fParams(params), bNeedEvent(false),
#ifdef USE_X264
		Encoder(params.width,params.height,params.fps)
#else
		Encoder2(params.width,params.height,params.fps)
#endif
{
	ptime.tv_sec = 0;
	ptime.tv_usec = 0;

	if (referenceCount == 0) {
		// Any global initialization of the device would be done here:
	}
	++referenceCount;

  // Any instance-specific initialization of the device would be done here:
  //%%% TO BE WRITTEN %%%

  // We arrange here for our "deliverFrame" member function to be called
  // whenever the next frame of data becomes available from the device.
  //
  // If the device can be accessed as a readable socket, then one easy way to do this is using a call to
  //     envir().taskScheduler().turnOnBackgroundReadHandling( ... )
  // (See examples of this call in the "liveMedia" directory.)
  //
  // If, however, the device *cannot* be accessed as a readable socket, then instead we can implement it using 'event triggers':
  // Create an 'event trigger' for this device (if it hasn't already been done):
  if (eventTriggerId == 0) {
	  eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
  }
}


Source::~Source() {
  // Any instance-specific 'destruction' (i.e., resetting) of the device would be done here:
  //%%% TO BE WRITTEN %%%

  --referenceCount;
  if (referenceCount == 0) {
    // Any global 'destruction' (i.e., resetting) of the device would be done here:
    source = NULL;

    // Reclaim our 'event trigger'
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
    eventTriggerId = 0;
  }
}


void Source::doGetNextFrame()
{
  // This function is called (by our 'downstream' object) when it asks for new data.

  // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
  if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
    handleClosure();
    return;
  }

  // If a new frame of data is immediately available to be delivered, then do this now:
  if (!fifo.empty()) {
    deliverFrame();
  }

  // No new data is immediately available to be delivered.  We don't do anything more here.
  // Instead, our event trigger must be called (e.g., from a separate thread) when new data becomes available.
  bNeedEvent = true;
}


void Source::deliverFrame0(void* clientData)
{
	((Source*)clientData)->deliverFrame();
}


void Source::deliverFrame()
{
  // This function is called when new frame data is available from the device.
  // We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
  // 'in' parameters (these should *not* be modified by this function):
  //     fTo: The frame data is copied to this address.
  //         (Note that the variable "fTo" is *not* modified.  Instead,
  //          the frame data is copied to the address pointed to by "fTo".)
  //     fMaxSize: This is the maximum number of bytes that can be copied
  //         (If the actual frame is larger than this, then it should
  //          be truncated, and "fNumTruncatedBytes" set accordingly.)
  // 'out' parameters (these are modified by this function):
  //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
  //     fNumTruncatedBytes: Should be set if the delivered frame would have been
  //         bigger than "fMaxSize", in which case it's set to the number of bytes
  //         that have been omitted.
  //     fPresentationTime: Should be set to the frame's presentation time
  //         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
  //         by calling "gettimeofday()".
  //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
  //         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
  //         to set this variable, because - in this case - data will never arrive 'early'.
  // Note the code below.

	int offset = 0;
	if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

	Buffer b = fifo.front();
	fifo.pop_front();

//    printf("read %d %d\n",b.len,fMaxSize);

    u_int8_t* newFrameDataStart = b.buf;
    unsigned newFrameSize = b.len;

    // Deliver the data here:
    if (newFrameSize > fMaxSize) {
    	fFrameSize = fMaxSize;
    	fNumTruncatedBytes = newFrameSize - fMaxSize;
    	offset = fMaxSize;
    } else {
    	fFrameSize = newFrameSize;
    	offset = 0;
    }

    if(ptime.tv_sec == 0 && ptime.tv_usec == 0) {
        gettimeofday(&ptime, NULL); // If you have a more accurate time - e.g., from an encoder - then use that instead.
    }
    fPresentationTime.tv_sec  = ptime.tv_sec + b.ts/1000000;
    fPresentationTime.tv_usec = ptime.tv_usec + b.ts%1000000;

    // If the device is *not* a 'live source' (e.g., it comes instead from a file or buffer), then set "fDurationInMicroseconds" here.
    memmove(fTo, newFrameDataStart, fFrameSize);
    if(offset) {
//    	printf("push\n");
    	fifo.push_front(Buffer(b.buf+offset,b.len-offset,b.ts));
    }

    //fDurationInMicroseconds = 1000*1000/fps;

    // After delivering the data, inform the reader that it is now available:
    FramedSource::afterGetting(this);
}



void Source::write(uint8_t *buf,int len,uint64_t ts)
{
    printf("write %d %ld\n",len,ts);

    fifo.push_back(Buffer(buf,len,ts));
    if(bNeedEvent) {
    	bNeedEvent = false;
    	envir().taskScheduler().triggerEvent(Source::eventTriggerId, this);
    }
}



