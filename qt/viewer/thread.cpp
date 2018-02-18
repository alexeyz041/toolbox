
#include "thread.h"

#include "serversocket.h"
#include "depacketizer.h"

#define PORT 8888   //The port on which to listen for incoming data


class Server : public ServerSocket {
public:
	Server(RenderThread *rt) : ServerSocket(), rt(rt)
	{
	}

	virtual void received(uint8_t *buf,int len)
	{
		dp.addPacket(buf,len);
		if(dp.isReady()) {
			uint32_t l = 0;
			uint8_t *p = dp.getData(&l);
			emit rt->received(p,l);
		}
	}

	DePacketizer dp;
	RenderThread *rt;
};



void RenderThread::run()
{
	Server s(this);
	s.listen(PORT,&quit);
}

void RenderThread::stop()
{
	quit = true;
	printf("stop rcvr\n");
}


