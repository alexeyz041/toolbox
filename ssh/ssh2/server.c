
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

#include "server.h"

//==============================================================================

void *server_session(void *p)
{
int count = 0;
unsigned char recvbuf[256] = {0};
worker_arg_t *arg = (worker_arg_t *)p;
int s = arg->sock;
void (*wcb)(void *, void *, int, int) = arg->wcb;
void *ctx = arg->ctx;
    free(p);

    for(count=0; ; count++) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(s, &fds);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	int result = select(s + 1 , &fds , NULL , NULL , &tv);
    	if (result > 0 && FD_ISSET(s, &fds)) {
	    result = recv(s, recvbuf, sizeof(recvbuf), 0);
            if (result > 0) {
                (*wcb)(ctx, recvbuf, result, s);
	    } else if (result == 0) {
        	printf("Connection closed by peer!\n");
		break;
	    } else {
        	printf("recv failed\n");
		break;
	    }
	}
	if(result < 0) {
	    printf("select failed\n");
	    break;
	}
    }
    close(s);
    return NULL;
}


int server_send(int s, uint8_t *sendbuf, int sendlen)
{
    return send(s, sendbuf, sendlen, 0);
}

//==============================================================================

void *server(void *p)
{
server_arg_t *arg = (server_arg_t *)p;
int s;
struct sockaddr_in sa, sa2;

    if(!arg) {
        return NULL;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf ("Server socket creation failed\n");
        free(arg);
        return NULL;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(arg->port);

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        printf("bind failed\n");
        close(s);
        free(arg);
        return NULL;
    }

    if(listen (s, 5) != 0) {
        printf("listen failed\n");
	close(s);
        free(arg);
	return NULL;
    }

    printf("server active at %d\n", arg->port);
    for ( ; ; ) {
        unsigned int len = sizeof(sa2);
        int s2 = accept(s, (struct sockaddr *)&sa2, &len);
        if (s2 == -1) {
	    printf("accept failed\n");
            break;
        }
        printf("Connection came from %s:%d\n", inet_ntoa(sa2.sin_addr), sa2.sin_port);

	pthread_t thread_id;
	worker_arg_t *arg2 = (worker_arg_t *)malloc(sizeof(worker_arg_t));
	arg2->sock = s2;
	arg2->wcb = arg->wcb;
	arg2->ctx = arg->ctx;
	pthread_create(&thread_id, NULL, server_session, arg2);
    }

    printf("server exit\n");
    close(s);
    free(arg);
    return NULL;
}


int start_server(int port, void (*wcb)(void *, void *, int, int), void *ctx)
{
pthread_t tid;
server_arg_t *arg = malloc(sizeof(server_arg_t));
    if(!arg) {
        return -1;
    }
    arg->port = port;
    arg->wcb = wcb;
    arg->ctx = ctx;
    pthread_create(&tid, NULL, server, arg);
    return 0;
}

