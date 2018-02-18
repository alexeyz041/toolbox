
#include "serversocket.h"


ServerSocket::ServerSocket()
{
	memset(&local_addr, 0, sizeof(local_addr));
	memset(&remote_addr, 0, sizeof(remote_addr));
    if ((fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
    }
    buf_size = 64*1024;
    buf = (uint8_t*)malloc(buf_size);
    if(!buf) {
    	fprintf(stderr,"serversocket: out of memory");
	exit(1);
    }
}


ServerSocket::~ServerSocket()
{
	if(fd != -1) {
		close(fd);
		fd = -1;
	}
	if(buf) {
		free(buf);
		buf = nullptr;
	}
}


int ServerSocket::listen(uint16_t port,bool *quit)
{
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    int rc = setsockopt(fd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0) {
       perror("setsockopt failed");
       return -1;
    }

    rc = ioctl(fd, FIONBIO, (char *)&on);
    if (rc < 0) {
       perror("ioctl failed");
       return -1;
    }

    //bind socket to port
    if(bind(fd, (struct sockaddr*)&local_addr, sizeof(local_addr) ) == -1) {
        perror("bind failed");
        return -1;
    }

    while(*quit != true) {
    	struct timeval timeout = { .tv_sec  = 1, .tv_usec = 0 };
    	fd_set set;

    	FD_ZERO(&set);
    	FD_SET(fd, &set);
    	rc = select(fd + 1, &set, NULL, NULL, &timeout);
    	if (rc < 0) {
    		perror("select failed");
    	    break;
    	} else  if(rc == 0) {
    	    printf(".\n");
    	    continue;
    	}

        socklen_t slen = sizeof(remote_addr);
        int recv_len = 0;
        if ((recv_len = recvfrom(fd, buf, buf_size, 0, (struct sockaddr *) &remote_addr, &slen)) == -1) {
            perror("recvfrom failed");
            break;
        }

        printf("Received packet from %s:%d length %d\n",
        				inet_ntoa(remote_addr.sin_addr),
						ntohs(remote_addr.sin_port),
						recv_len);
        received(buf,recv_len);
    }

    return -1;
}








