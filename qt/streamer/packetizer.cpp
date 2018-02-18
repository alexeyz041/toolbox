
#include "packetizer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define	MAX_PKT_LEN	(65507-sizeof(PKT_HEADER))

static uint8_t magic[] = { 14,2,3,4, 5,6,7,41 };


Packetizer::Packetizer(uint8_t *buf,uint32_t len) : buf(buf),len(len),pkt(nullptr)
{
	pkt = (uint8_t *)malloc(MAX_PKT_LEN+sizeof(PKT_HEADER));
	if(!pkt) {
		fprintf(stderr,"out of memory");
	}
}

Packetizer::~Packetizer()
{
	if(pkt) {
		free(pkt);
		pkt = nullptr;
	}
}

int Packetizer::getNumPackets(void)
{
	return 1 + len / MAX_PKT_LEN;
}



uint8_t *Packetizer::getPacket(int ix,uint32_t *pktlen)
{
PKT_HEADER *hdr = (PKT_HEADER *)pkt;
int nPkts = getNumPackets();
uint32_t offset = ix * MAX_PKT_LEN;
uint32_t frag_len = (ix != nPkts-1) ? MAX_PKT_LEN : (len - offset);

	memcpy(hdr->magic,magic,sizeof(hdr->magic));
	hdr->total_length[0] = (len & 0xff);
	hdr->total_length[1] = ((len >> 8) & 0xff);
	hdr->total_length[2] = ((len >> 16) & 0xff);
	hdr->total_length[3] = ((len >> 24) & 0xff);
	hdr->length[0] = (frag_len & 0xff);
	hdr->length[1] = ((frag_len >> 8) & 0xff);
	hdr->pkt_ix = ix;
	hdr->pkt_num = nPkts;

	memcpy(pkt+sizeof(PKT_HEADER), buf+offset, frag_len);
	*pktlen = frag_len + sizeof(PKT_HEADER);
	return pkt;
}

