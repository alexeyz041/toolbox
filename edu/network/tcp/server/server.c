
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


typedef struct {
    int port;
    int sock;
} arg_t;


int temp = 10000;
int delta = 100;

int get_temp()
{
    if(temp > 30000 || temp < 10000) {
	delta = -delta;
    }
    temp += delta;
    return temp;
}


void get_msg(unsigned char *buf,int *len)
{
int id = 0x659;
int temp = get_temp();

    buf[0] = 'C';

    buf[1] = id & 0xff;
    buf[2] = (id >> 8) & 0xff;
    buf[3] = (id >> 16) & 0xff;
    buf[4] = (id >> 24) & 0xff;

    buf[5] = 2;
    buf[6] = 8;		//msg len

    buf[7] = 0;
    buf[8] = 0;
    buf[9] = 0;
    buf[10] = 0;

    buf[11] = temp & 0xff;
    buf[12] = (temp >> 8) & 0xff;
    buf[13] = (temp >> 16) & 0xff;
    buf[14] = (temp >> 24) & 0xff;

    *len = 15;
}


//==============================================================================

void *server_session(void *p)
{
int count = 0;
unsigned char recvbuf[64] = {0};
arg_t *arg = (arg_t *)p;
int s2 = arg->sock;
int port = arg->port;
    free(p);

    for( ; ; count++) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(s2, &fds);

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	int result = select(s2 + 1 , &fds , NULL , NULL , &tv);
    	if (result > 0 && FD_ISSET(s2, &fds)) {
	    result = recv(s2, recvbuf, sizeof(recvbuf), 0);
            if (result > 0) {
	        printf("rcvd %d %2d: ",port,result);
	        for(int i=0; i < result; i++) {
		    printf("%02x ",recvbuf[i]);
		}
		printf("\n");
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

	if(count > 1000) {
	    unsigned char sendbuf[64] = {0};
	    int sendlen = sizeof(sendbuf);
	    get_msg(sendbuf,&sendlen);
            result = send(s2, sendbuf, sendlen, 0 );
    	    if (result == -1) {
		printf("send failed\n");
	    } else {
		printf("sent %d %2d: ",port,result);
	        for(int i=0; i < result; i++) {
		    printf("%02x ",sendbuf[i]);
		}
		printf("\n");
	    }
	    count = 0;
	}

    }

    close(s2);
    return NULL;
}


//==============================================================================

void *server(void *p)
{
arg_t *arg = (arg_t *)p;
int s;
struct sockaddr_in sa, sa2;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf ("Socket creation failed\n");
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
        return NULL;
    }

    if(listen (s, 5) != 0) {
        printf("listen failed\n");
	close(s);
	return NULL;
    }

    for ( ; ; ) {
        unsigned int len = sizeof(sa2);
        int s2 = accept (s, (struct sockaddr *)&sa2, &len);
        if (s2 == -1) {
	    printf("accept failed\n");
            break;
        }
        printf("Connection came from %s:%d\n", inet_ntoa(sa2.sin_addr), sa2.sin_port);

	pthread_t thread_id;
	arg_t *arg2 = (arg_t *)malloc(sizeof(arg_t));
	arg2->sock = s2;
	arg2->port = arg->port;
	pthread_create(&thread_id, NULL, server_session, arg2);
    }

    close(s);
    return 0;
}


int main(int argc, char **argv)
{
pthread_t tid1;
pthread_t tid2;
arg_t arg1 = { 30000 };
arg_t arg2 = { 30001 };

//    pthread_create(&tid1, NULL, server, &arg1);
    pthread_create(&tid2, NULL, server, &arg2);
//    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    return 0;
}

