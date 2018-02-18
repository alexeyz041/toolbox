
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "packetizer.h"
#include "screenshot.h"
#include "crop.h"
#include "png.h"

#define SERVER 	"127.0.0.1"
#define PORT 	8888
#define WIDTH	640
#define	HEIGHT	480


uint8_t *load(const char *fnm,int *len)
{
	FILE *f = fopen(fnm,"rb");
	if(!f) return NULL;

	fseek(f,0,SEEK_END);
	int length = ftell(f);
	fseek(f,0,SEEK_SET);
	uint8_t *p = (uint8_t *)malloc(length);
	if(!p) {
		fclose(f);
		return NULL;
	}
	if(fread(p,1,length,f) != length) {
		fprintf(stderr,"load from %s failed\n",fnm);
	}
	fclose(f);
	*len = length;
	return p;
}


void save(const char *fnm,uint8_t *buf,int len)
{
	FILE *f = fopen(fnm,"wb");
	if(!f) return;
	fwrite(buf,1,len,f);
	fclose(f);
}



void die(const char *s)
{
    perror(s);
    exit(1);
}


uint8_t zbuf[WIDTH*HEIGHT*BPP];


int main(void)
{
    struct sockaddr_in remote_addr;
    int slen=sizeof(remote_addr);
    int fd = -1;

    if((fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket");
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);
    if (inet_aton(SERVER , &remote_addr.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    if(connect(fd,(const sockaddr*)&remote_addr,sizeof(remote_addr)) == -1) {
        die("connect");
    }

    ScreenShot shot;
    Crop *cr = nullptr;
    Png *png = nullptr;
    for(;;) {
    	int w = 0;
    	int h = 0;
    	uint32_t *buf = nullptr;
    	shot.get(&w,&h,&buf);
    	if(!png) {
    		cr = new Crop(w,h,0,0,WIDTH,HEIGHT);
    		png = new Png(WIDTH,HEIGHT);
    	}
    	cr->crop((uint8_t *)buf, w*h*BPP, zbuf, sizeof(zbuf));

    	uint8_t *obuf = nullptr;
    	int olen = 0;
    	png->compress((uint8_t *)zbuf,&obuf,&olen);
    	//save("1.png",obuf,olen);

    	Packetizer p(obuf,olen);

    	int nPkts = p.getNumPackets();
    	for(int i=0; i < nPkts; i++) {
    		uint32_t pktlen = 0;
    		uint8_t *pkt = p.getPacket(i,&pktlen);
    		printf("pktlen = %d\n",pktlen);
    		if(send(fd, pkt, pktlen , 0) != pktlen) {
    			die("send");
    		}
    		usleep(1000);
    	}
    	usleep(1000*1000);
    }

    close(fd);
    return 0;
}

