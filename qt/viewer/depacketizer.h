
#ifndef DEPACKETIZER_H
#define DEPACKETIZER_H

#include <stdint.h>

typedef struct {
	uint8_t magic[8];
	uint8_t	total_length[4];
	uint8_t	length[2];
	uint8_t pkt_ix;
	uint8_t pkt_num;
} PKT_HEADER;


enum DP_State {
	DP_IDLE,
	DP_FRAGMENT,
	DP_READY
};



class DePacketizer {
public:
	DePacketizer();
	~DePacketizer();

	void addPacket(uint8_t *buf,uint32_t len);
	bool isReady();
	uint8_t *getData(uint32_t *len);

private:

private:
	uint8_t *buf;
	uint32_t length;
	uint32_t total_length;
	uint8_t pkt_ix;
	uint8_t pkt_num;
	DP_State state;
};


#endif //DEPACKETIZER_H


