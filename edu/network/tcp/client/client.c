
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
    char ip[16];
    int port;
} arg_t;


int cnt = 0;

void get_msg(unsigned char *sendbuf,int *sendlen)
{
    sendbuf[0] = 1;
    sendbuf[1] = 2;
    sendbuf[2] = 3;
    sendbuf[3] = cnt++;
    *sendlen = 4;
}


//==============================================================================

void client_session(int s2,arg_t *arg)
{
int count = 0;
unsigned char recvbuf[64] = {0};

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
	        printf("%s:%d rcvd %2d: ", arg->ip,arg->port,result);
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
/*
	if(count > 1000) {
	    unsigned char sendbuf[64] = {0};
	    int sendlen = sizeof(sendbuf);
	    get_msg(sendbuf,&sendlen);
            result = send(s2, sendbuf, sendlen, 0 );
    	    if (result == -1) {
		printf("send failed\n");
	    } else {
		printf("sent %2d: ",result);
	        for(int i=0; i < result; i++) {
		    printf("%02x ",sendbuf[i]);
		}
		printf("\n");
	    }
	    count = 0;
	}
*/
    }
}

//==============================================================================

void *client(void *p)
{
arg_t *arg = (arg_t *)p;
int s;
struct sockaddr_in sa;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf ("Socket creation failed\n");
        return NULL;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, arg->ip, &sa.sin_addr);
    sa.sin_port = htons(arg->port);

    printf("->%s:%d\n",arg->ip,arg->port);

    if(connect(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
       printf("Connect failed\n");
       return NULL;
    } 

    client_session(s,arg);

    close(s);
    return NULL;
}


int main(int argc, char **argv)
{
pthread_t tid[2] = {0};

    arg_t arg1 = { "192.168.201.1", 30001 };
    pthread_create(&tid[0], NULL, client, &arg1);

    arg_t arg2 = { "192.168.201.1", 30000 };
    pthread_create(&tid[1], NULL, client, &arg2);


    for(int i=0; i < 2; i++) {
	pthread_join(tid[i],NULL);
    }
    
    return 0;
}

