
#include <stdlib.h>

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <ByteStreamMemoryBufferSource.hh>

#include "streamer.h"
#include "source.h"
#include "mediasubsession.h"
#include "screenshot.h"
#include "scaler.h"
#include "crop.h"
#include "camera.h"

UsageEnvironment *env = nullptr;
H264VideoStreamFramer *videoSource = nullptr;
RTPSink *videoSink = nullptr;
Source *source = nullptr;

ScreenShot shot;
char cam_device[64] = "/dev/video0";
Camera *cam = nullptr;
Crop *croper = nullptr;
Scaler *scaler = nullptr;

int screen = 1;
int mcast = 1;
int crop = 1;
int width = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;
int x = 0;
int y = 0;
int fps = DEFAULT_FPS;
uint64_t ts = 0;

uint8_t *frame_buffer = nullptr;
int frame_buffer_length = 0;



void Usage()
{
	printf("RTSP desktop/camera streaming utility\n\n");
	printf("rtsp-streamer [-camera <name>/-desktop] [-mc/-od] [-h] [-scale WxH/-crop WxH+X+Y]\n\n");
	printf("-camera device  steam image from camera device\n");
	printf("-desktop        stream image from desktop (default)\n");
	printf("-mc             use multicast server (default)\n");
	printf("-od             use ondemand server\n");
	printf("-scale WxH      scale image to WxH\n");
	printf("-crop WxH+X+Y   crop image to WxH from X,Y (default 640:480+0+0)\n");
	printf("-h              print this information\n");
	printf("\n");
	exit(1);
}


int getGeometry(const char *g)
{
	const char *temp = strchr(g,'x');
	if(!temp) return -1;
	width = atoi(g);
	height = atoi(++temp);
	if(crop) {
		temp = strchr(temp,'+');
		if(!temp) return -1;
		x = atoi(++temp);
		temp = strchr(temp,'+');
		if(!temp) return -1;
		y = atoi(++temp);
	}
	if(width == 0 || height == 0) return -1;
	return 0;
}


int main(int argc, char** argv)
{
	for(int i=1; i < argc; i++) {
		if(strcasecmp(argv[i],"-camera") == 0) {
			if(i+1 >= argc) Usage();
			screen = 0;
			snprintf(cam_device,sizeof(cam_device),"%s",argv[i+1]);
			i++;
		} else if(strcasecmp(argv[i],"-desktop") == 0) {
			screen = 1;
		} else if(strcasecmp(argv[i],"-scale") == 0) {
			if(i+1 >= argc) Usage();
			crop = 0;
			if(getGeometry(argv[i+1]) != 0) Usage();
			i++;
		} else if(strcasecmp(argv[i],"-crop") == 0) {
			if(i+1 >= argc) Usage();
			crop = 1;
			if(getGeometry(argv[i+1]) != 0) Usage();
			i++;
		} else if(strcasecmp(argv[i],"-mc") == 0) {
			mcast = 1;
		} else if(strcasecmp(argv[i],"-od") == 0) {
			mcast = 0;
		} else if(strcasecmp(argv[i],"-h") == 0) {
			Usage();
		} else {
			Usage();
		}
	}

	printf("source %s %s\n", screen ? "screen" : "camera", screen ? "" : cam_device);
	printf("mode %s\n", mcast ? "multicast" : "ondemand");
	if(crop) {
		printf("crop to %dx%d+%d+%d\n",width,height,x,y);
	} else {
		printf("scale to %dx%d\n",width,height);
	}

  	if(!screen) {
  		cam = new Camera(width,height,cam_device);
  	}

  	frame_buffer_length = Scaler::getBufferSize(width,height);
  	frame_buffer = new uint8_t[frame_buffer_length + 32];
  	if(!frame_buffer) {
  		*env << "can\'t allocate frame buffer\n";
  		exit(1);
  	}

	if(mcast) Multicast(); else onDemand();
	return 0;
}


void Multicast()
{
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	struct in_addr destinationAddress;
	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);

	const unsigned short rtpPortNum = 18888;
	const unsigned short rtcpPortNum = rtpPortNum+1;
	const unsigned char ttl = 255;

	const Port rtpPort(rtpPortNum);
	const Port rtcpPort(rtcpPortNum);

	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
	rtpGroupsock.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
	rtcpGroupsock.multicastSendOnly(); // we're a SSM source

	// Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 200000;
	videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

	// Create (and start) a 'RTCP instance' for this RTP sink:
	const unsigned estimatedSessionBandwidth = 1500; // in kbps; for RTCP b/w share

	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen+1];
	gethostname((char *)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';

	RTCPInstance* rtcp = RTCPInstance::createNew(*env,
		  	  	  	  	  	  	  	  	  	  &rtcpGroupsock,
											  estimatedSessionBandwidth,
											  CNAME,
											  videoSink,
											  NULL /* we're a server */,
											  True /* we're a SSM source */);

	// Note: This starts RTCP running automatically
	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	ServerMediaSession* sms = ServerMediaSession::createNew(*env,
		  	  	  	  	  	  	  	  	  	  	  "testStream",
												  "testStreamInfo",
												  "Session streamed by testStreamer",
												  True /*SSM*/);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	rtspServer->addServerMediaSession(sms);

	char* url = rtspServer->rtspURL(sms);
	*env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;

   	*env << "Beginning streaming...\n";
  	play();

  	env->taskScheduler().doEventLoop();	// does not return
}

//=============================================================================

#if 0
void afterPlaying(void* /*clientData*/)
{
  *env << "...should not happen\n";

  videoSink->stopPlaying();
  Medium::close(videoSource);
  if(scaler) {
	  delete scaler;
	  scaler = nullptr;
  }
  if(croper) {
	  delete croper;
	  croper = nullptr;
  }
  if(cam) {
	  delete cam;
	  cam = nullptr;
  }
  // Note that this also closes the input file that this source read from.
  // Start playing once again:
  play();
}
#endif


void sendData(void *p)
{
	if(screen) {
		int w = width;
		int h = height;
		uint32_t *b = nullptr;
		shot.get(&w,&h,&b);
		if(crop) {
			if(!croper) {
				croper = new Crop(w,h,0,0,width,height);
			}
			croper->crop((uint8_t*)b,w*h*BPP, frame_buffer,frame_buffer_length);
		} else {
			if(!scaler) {
				scaler = new Scaler(w,h,width,height);
			}
			scaler->scale((uint8_t*)b,w*h*BPP, frame_buffer,frame_buffer_length);
		}
	} else {
		cam->get(frame_buffer,frame_buffer_length);
	}

	if(source) {
		source->encode(frame_buffer,frame_buffer_length,ts);
	}

	uint64_t frame_time = 1000*1000/fps;
	frame_time -= (frame_time/20);
	ts += frame_time;
	env->taskScheduler().scheduleDelayedTask(frame_time,sendData,NULL);
}


void play()
{
  // Open the input file as a 'byte-stream file source':
  source  = Source::createNew(*env, SourceParameters(width,height,fps));
  if (source == NULL) {
    *env << "Unable create source\n";
    exit(1);
  }

  env->taskScheduler().scheduleDelayedTask(100,sendData,NULL);

  FramedSource* videoES = source;

  // Create a framer for the Video Elementary Stream:
  videoSource = H264VideoStreamFramer::createNew(*env, videoES);

  // Finally, start playing:
  //*env << "Beginning to read from file...\n";
  videoSink->startPlaying(*videoSource, /*afterPlaying*/ nullptr, videoSink);
}

//=============================================================================

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
const Boolean reuseFirstSource = True;



int onDemand()
{
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  char const* descriptionString = "Session streamed by testStreamer";

  // A H.264 video elementary stream:
  char const* streamName = "testStream";
  ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName,  descriptionString);
  sms->addSubsession(H264VideoServerMediaSubsession::createNew(*env, SourceParameters(width,height,fps), reuseFirstSource));
  rtspServer->addServerMediaSession(sms);

  announceStream(rtspServer, sms, streamName);

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
    *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().scheduleDelayedTask(100,sendData,NULL);

  env->taskScheduler().doEventLoop(); // does not return
}


void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName)
{
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();
	env << "\n\"" << streamName << "\" stream \n";
	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}


