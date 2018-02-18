#ifndef STREAMER_H
#define STREAMER_H

#define	DEFAULT_WIDTH	640
#define	DEFAULT_HEIGHT	480
#define DEFAULT_FPS 	10

void Multicast(void);
int onDemand(void);
void play(void);

void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms, char const* streamName);


#endif //STREAMER_H
