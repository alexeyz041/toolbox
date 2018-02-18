
#include "depacketizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define	MAX_PKT_LEN	(65507-sizeof(PKT_HEADER))

static uint8_t magic[] = { 14,2,3,4, 5,6,7,41 };


DePacketizer::DePacketizer() : buf(nullptr),length(0),total_length(0),pkt_ix(0),pkt_num(0),state(DP_IDLE)
{
}

DePacketizer::~DePacketizer()
{
	if(buf) {
		free(buf);
		buf = nullptr;
	}
}



/*
uint8_t *Packetizer::getPacket(int ix,uint32_t *pktlen,PKT_HEADER *hdr)
{
int nPkts = getNumPackets();
uint32_t offset = ix * MAX_PKT_LEN;

	*pktlen = (ix != nPkts-1) ? MAX_PKT_LEN : len - offset;

	memcpy(hdr->magic,magic,sizeof(hdr->magic));
	hdr->total_length[0] = (len & 0xff);
	hdr->total_length[1] = ((len >> 8) & 0xff);
	hdr->total_length[2] = ((len >> 16) & 0xff);
	hdr->total_length[3] = ((len >> 24) & 0xff);
	hdr->length[0] = (*pktlen & 0xff);
	hdr->length[1] = ((*pktlen >> 8) & 0xff);
	hdr->pkt_ix = ix;
	hdr->pkt_num = nPkts;

	return buf+offset;
}
*/



void DePacketizer::addPacket(uint8_t *buffer,uint32_t len)
{
PKT_HEADER hdr;

	printf("state = %d\n",state);
	if(state == DP_READY) return;
	if(len <= sizeof(hdr)) {
		state = DP_IDLE;
		return;
	}

	memcpy(&hdr,buffer,sizeof(hdr));
	if(memcmp(hdr.magic,magic,sizeof(magic)) != 0) {
		state = DP_IDLE;
		return;
	}

	uint32_t tmp = total_length;
	total_length = hdr.total_length[0] + (hdr.total_length[1] << 8) + (hdr.total_length[2] << 16) + (hdr.total_length[3] << 24);
	length = hdr.length[0] + (hdr.length[1] << 8);
	uint8_t oix = pkt_ix;
	pkt_ix = hdr.pkt_ix;
	pkt_num = hdr.pkt_num;

	if(state == DP_IDLE) {
		if(pkt_ix != 0) return;
		if(total_length > tmp) {
			buf = (uint8_t *)realloc(buf, total_length);
		}
	} else if(state == DP_FRAGMENT) {
		if(pkt_ix != oix+1) {
			state = DP_IDLE;
			return;
		}
	}

	uint32_t offset = pkt_ix * MAX_PKT_LEN;
	bool bLast = false;

	if(length != len-sizeof(hdr)) {
		state = DP_IDLE;
		return;
	}

	if(pkt_ix == pkt_num-1) {
		bLast = true;
		if(length != total_length-offset) {
			state = DP_IDLE;
			return;
		}
	} else {
		if(length != MAX_PKT_LEN) {
			state = DP_IDLE;
			return;
		}
	}
	memcpy(buf+offset, buffer+sizeof(hdr), length);
	state = bLast ? DP_READY: DP_FRAGMENT;
}



bool DePacketizer::isReady()
{
	return (state == DP_READY);
}

uint8_t *DePacketizer::getData(uint32_t *len)
{
	*len = total_length;
	state = DP_IDLE;
	printf("get data\n");
	return buf;
}


