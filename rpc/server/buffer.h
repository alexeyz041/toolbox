
#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include "types.h"

#define	ADD(X,Y)	X.put(&Y,sizeof(Y))
#define	EXT(X,Y)	X.get(&Y,sizeof(Y))


class Buffer {
public:

	Buffer(uint8_t *buf,int len) : buf(buf),len(len),pos(0)
	{
	}

	int length()
	{
		return pos;
	}

	void put(void *p,int len)
	{
		memcpy(buf+pos,p,len);
		pos += len;
	}

	void get(void *p,int len)
	{
		memcpy(p,buf+pos,len);
		pos += len;
	}

private:

	uint8_t *buf;
	int len;
	int pos;
};



#endif // BUFFER_H_INCLUDED

