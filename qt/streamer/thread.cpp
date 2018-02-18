
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

#define SERVER "127.0.0.1"
#define PORT 8888


void shot(QByteArray& bytes)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	QPixmap orig = screen->grabWindow(0);
	QRect rect(0, 0, 832, 640);
	QPixmap cropped = orig.copy(rect);
	QImage img = cropped.toImage();

	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	img.save(&buffer, "PNG");
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

	for(;;) {
		QByteArray bytes;
		shot(bytes);
		Packetizer p((uint8_t *)bytes.data(),bytes.size());

		int nPkts = p.getNumPackets();
		for(int i=0; i < nPkts; i++) {
			uint32_t pktlen = 0;
			uint8_t *pkt = p.getPacket(i,&pktlen);
			printf("pktlen = %d\n",pktlen);
			if(send(fd, pkt, pktlen , 0) != pktlen) {
				fprintf(stderr, "send failed\n");
				//exit(1);
			}
			usleep(1000);
		}
		usleep(1000*100);
	}

	close(fd);
}


void RenderThread::stop()
{
	quit = true;
}

