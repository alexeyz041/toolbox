
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>


#define DEFAULT_BUFLEN 512


void *server(void *p)
{
int result;
int recvbuflen = DEFAULT_BUFLEN;
unsigned char recvbuf[DEFAULT_BUFLEN] = "";

int *s = (int *)p;
int s2 = *s;
    free(p);
    for(;;) {
	result = recv(s2, recvbuf, recvbuflen, 0);
        if (result > 0) {
            printf("%d Bytes received from sender\n", result);
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

	if(result > 0) {
            result = send(s2, recvbuf, result, 0 );
    	    if (result == -1) {
		printf("send failed\n");
	    } else {
		printf("sent %d bytes back\n",result);
	    }
	}
    }
    close(s2);
    return NULL;
}





int main(int argc, char **argv)
{
int s;
struct sockaddr_rc sab, sab2;

    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (s == -1) {
        printf ("Socket creation failed\n");
        return 1;
    }

    memset(&sab, 0, sizeof(sab));
    sab.rc_family = AF_BLUETOOTH;
//    sab.rc_bdaddr = BDADDR_ANY;
    sab.rc_channel = 1;

    if (bind(s, (struct sockaddr *)&sab, sizeof(sab)) != 0) {
        printf ("bind failed\n");
        close(s);
        return 1;
    }

    if(listen (s, 5) != 0) {
        printf("listen failed\n");
	close(s);
	return 1;
    }

    for ( ; ; ) {
        unsigned int ilen = sizeof(sab2);
        int s2 = accept (s, (struct sockaddr *)&sab2, &ilen);
        if (s2 == -1) {
	    printf ("accept failed\n");
            break;
        }
        printf ("Connection came from %s to channel %d\n", batostr(&sab2.rc_bdaddr), sab2.rc_channel);

	pthread_t thread_id;
	int *arg = (int *)malloc(sizeof(int));
	*arg = s2;
	pthread_create(&thread_id, NULL, server, arg);
    }

    close(s);
    return 0;
}

