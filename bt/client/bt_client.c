
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define DEFAULT_BUFLEN 512



int main(int argc, char **argv)
{
int s;
struct sockaddr_rc sab;
int result;
char *sendBuf = "Test data from client...";
int recvBufLen = DEFAULT_BUFLEN;
unsigned char recvBuf[DEFAULT_BUFLEN] = "";

    s = socket (AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (s == -1) {
        printf("Socket creation failed\n");
        return 1;
    }

    memset(&sab, 0, sizeof(sab));
    sab.rc_family = AF_BLUETOOTH;
//    str2ba("9C:B6:D0:E2:2C:64", &sab.rc_bdaddr);
    str2ba("F4:8C:50:03:4B:65", &sab.rc_bdaddr);
    sab.rc_channel = 1;

    if (connect (s, (struct sockaddr *)&sab, sizeof(sab)) == -1) {
        printf("connect failed\n");
        close(s);
        return 1;
    }

    result = send(s, sendBuf, (int)strlen(sendBuf), 0 );
    if (result == -1) {
        printf("send failed\n");
        close(s);
        return 1;
    }
    printf("send OK\n");
//    shutdown(s,SHUT_WR);

    // Receive until the peer closes the connection
    result = recv(s, recvBuf, recvBufLen, 0);
    if (result > 0) {
        printf(" %d Bytes received from sender\n", result);
        for(int i=0; i < result; i++)
	    printf("%02x ",recvBuf[i]);
	printf("\n");
    } else if (result == 0) {
        printf("Connection was closed by peer!\n");
    } else {
        printf("recv() failed\n");
    }

    close(s);
    return 0;
}
