
#include "serversocket.h"
#include "screen.h"
#include "depacketizer.h"

#define PORT 8888


class Server : public ServerSocket {
public:
	Server() : ServerSocket(), screen(832,640)
	{
	}

	virtual void received(uint8_t *buf,int len)
	{
		dp.addPacket(buf,len);
		if(dp.isReady()) {
			uint32_t l = 0;
			uint8_t *p = dp.getData(&l);
			screen.show(p,l);
		}
	}

	DePacketizer dp;
	Screen screen;
};


int main(void)
{
	Server s;
	s.listen(PORT,&s.screen.quit);
    return 0;
}

