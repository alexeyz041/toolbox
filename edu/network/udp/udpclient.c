
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, char **argv)
{
int sockfd, portno, n;
int serverlen;
struct sockaddr_in serveraddr;
struct hostent *server;
char *hostname;
char buf[BUFSIZE] = "Hello Network!";

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }

    hostname = argv[1];
    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf)+1, 0, (struct sockaddr*)&serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&serveraddr, &serverlen);
    if (n < 0) 
      error("ERROR in recvfrom");
    buf[n] = '\0';
    printf("Echo from server: %s\n", buf);

    return 0;
}
