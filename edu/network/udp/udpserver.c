
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void error(char *msg)
{
  perror(msg);
  exit(1);
}


int main(int argc, char **argv)
{
int sockfd;
int portno; /* port to listen on */
int clientlen; /* byte size of client's address */
struct sockaddr_in serveraddr; /* server's addr */
struct sockaddr_in clientaddr; /* client addr */
struct hostent *hostp; /* client host info */

char buf[BUFSIZE]; /* message buf */
char *hostaddrp; /* dotted decimal host addr string */
int optval; /* flag value for setsockopt */
int n;

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(1);
    }
    portno = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
	error("ERROR opening socket");

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
	error("ERROR on binding");

    clientlen = sizeof(clientaddr);
    for(;;) {
	bzero(buf, BUFSIZE);
	n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
	if (n < 0)
	    error("ERROR in recvfrom");
	buf[n] = '\0';

	hostaddrp = inet_ntoa(clientaddr.sin_addr);
	printf("server received %d/%d bytes: %s\n", (int)strlen(buf), n, buf);

	n = sendto(sockfd, buf, strlen(buf)+1, 0, (struct sockaddr *) &clientaddr, clientlen);
	if (n < 0) 
    	    error("ERROR in sendto");
    }
    return 0;
}

