
#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>


class ServerSocket {
public:
	ServerSocket();
	virtual ~ServerSocket();

	int listen(uint16_t port, bool *quit);
	virtual void received(uint8_t *buf,int len) = 0;

private:
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    int fd;
    uint8_t *buf;
    int buf_size;
};

#endif //SERVERSOCKET_H

