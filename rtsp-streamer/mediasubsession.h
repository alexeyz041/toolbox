
#ifndef H264_VIDEO_SERVER_MEDIA_SUBSESSION_H
#define H264_VIDEO_SERVER_MEDIA_SUBSESSION_H

#ifndef _SERVER_MEDIA_SESSION_HH
#include "ServerMediaSession.hh"
#endif

#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif


class H264VideoServerMediaSubsession: public OnDemandServerMediaSubsession {
public:
  static H264VideoServerMediaSubsession *createNew(UsageEnvironment& env,
		  	  	  	  	  	  	  	  	  	  	  SourceParameters params,
												  Boolean reuseFirstSource);

  // Used to implement "getAuxSDPLine()":
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();

protected:
  H264VideoServerMediaSubsession(UsageEnvironment& env,SourceParameters params, Boolean reuseFirstSource);
      // called only by createNew();
  virtual ~H264VideoServerMediaSubsession();

  void setDoneFlag() { fDoneFlag = ~0; }

protected: // redefined virtual functions
  virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

private:
  char* fAuxSDPLine;
  char fDoneFlag; // used when setting up "fAuxSDPLine"
  RTPSink* fDummyRTPSink; // ditto
  SourceParameters params;
};

#endif //H264_VIDEO_SERVER_MEDIA_SUBSESSION_H

