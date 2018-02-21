
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QBuffer>

#include "thread.h"
#include "packetizer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <time.h>

#define SERVER "127.0.0.1"
#define PORT 8888


// call this function to start a nanosecond-resolution timer
struct timespec timer_start()
{
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time)
{
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}



void shot(QByteArray& bytes)
{
	struct timespec start = timer_start();

	QScreen *screen = QGuiApplication::primaryScreen();
	QPixmap orig = screen->grabWindow(0);
	QRect rect(0, 0, 832, 640);
	QPixmap cropped = orig.copy(rect);
	QImage img = cropped.toImage();

	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
//	img.save(&buffer, "BMP");
	img.save(&buffer, "PNG");
//	img.save(&buffer, "JPG");

	long delta = timer_end(start)/100000;
	printf("png %ld.%d\n",delta/10,delta%10);
}


void RenderThread::run()
{
	struct sockaddr_in remote_addr;
	int fd = -1;

	if((fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		fprintf(stderr, "socket failed\n");
		exit(1);
	}

	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(PORT);
	if (inet_aton(SERVER , &remote_addr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton failed\n");
		exit(1);
	}

	if(::connect(fd,(const sockaddr*)&remote_addr,sizeof(remote_addr)) == -1) {
		fprintf(stderr, "connect failed\n");
		exit(1);
	}

	double nw = 0;
	int count = 0;
	for(;;) {
		struct timespec start2 = timer_start();
		QByteArray bytes;
		shot(bytes);

		struct timespec start = timer_start();
		Packetizer p((uint8_t *)bytes.data(),bytes.size());
		int nPkts = p.getNumPackets();
		for(int i=0; i < nPkts; i++) {
			uint32_t pktlen = 0;
			uint8_t *pkt = p.getPacket(i,&pktlen);
			//printf("pktlen = %d\n",pktlen);
			if(send(fd, pkt, pktlen , 0) != pktlen) {
				fprintf(stderr, "send");
				//exit(1);
			}
			nw += pktlen;
			usleep(1000);
		}
		long delta = timer_end(start)/100000;
		long delta2 = timer_end(start2)/1000000;

		count++;
		printf("send %ld.%d wait %d network %3.1fKB/s\n",delta/10,delta%10,delta2,(10*nw/1024/count));

		if(delta2 > 100) delta2 = 100;
		usleep(1000*(100-delta2));
	}

	close(fd);
}


void RenderThread::stop()
{
	quit = true;
}

