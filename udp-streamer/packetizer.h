
#ifndef PACKETIZER_H
#define PACKETIZER_H

#include <stdint.h>

typedef struct {
	uint8_t magic[8];
	uint8_t	total_length[4];
	uint8_t	length[2];
	uint8_t pkt_ix;
	uint8_t pkt_num;
} PKT_HEADER;



class Packetizer {
public:
	Packetizer(uint8_t *buf,uint32_t len);
	~Packetizer();

	int getNumPackets(void);
	uint8_t *getPacket(int ix,uint32_t *len);

private:
	uint8_t *buf;
	uint32_t len;
	uint8_t *pkt;
};


#endif //PACKETIZER_H


