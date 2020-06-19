
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 
#include <pthread.h>

#include "client.h"

//==============================================================================

CLIENT_SESSION *client_open(const char *ip, int port)
{
int s;
struct sockaddr_in sa;
CLIENT_SESSION *session;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf ("Socket creation failed\n");
        return NULL;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sa.sin_addr);
    sa.sin_port = htons(port);

    printf("->%s:%d\n", ip, port);

    if(connect(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        printf("Connect to %s:%d failed\n", ip, port);
        close(s);
        return NULL;
    }

    session = malloc(sizeof(CLIENT_SESSION));
    if(!session) {
        printf("Out of memory\n");
        close(s);
        return NULL;
    }

    snprintf(session->ip, sizeof(session->ip), "%s", ip);
    session->port = port;
    session->fd = s;

    return session;
}

//==============================================================================

int client_read(CLIENT_SESSION *session, void *recvbuf, int recvbuf_len)
{
int count;
    if(!session || !recvbuf) {
        return -1;
    }

    for(count=0; count < RECV_TIMEOUT; count++) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(session->fd, &fds);

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000000;
	int result = select(session->fd + 1 , &fds , NULL , NULL , &tv);
    	if (result > 0 && FD_ISSET(session->fd, &fds)) {
	    result = recv(session->fd, recvbuf, recvbuf_len, 0);
            if (result > 0) {
                return result;
	    } else if (result == 0) {
        	printf("Connection closed by peer!\n");
		return -1;
	    } else {
        	printf("recv failed\n");
		return -1;
	    }
	}

	if(result < 0) {
	    printf("select failed\n");
	    return -1;
	}
    }
    printf("recv timeout\n");
    return -1;
}

//==============================================================================

int client_send(CLIENT_SESSION *session, void *sendbuf, int sendlen)
{
    if(!session) {
        return -1;
    }
    return send(session->fd, sendbuf, sendlen, 0 );
}

//==============================================================================

void client_close(CLIENT_SESSION *session)
{
    if(session) {
        close(session->fd);
        free(session);
    }
}

