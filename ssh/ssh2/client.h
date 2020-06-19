
#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#define RECV_TIMEOUT 30 //sec

typedef struct {
    char ip[64];
    int port;
    int fd;
} CLIENT_SESSION;

CLIENT_SESSION *client_open(const char *ip, int port);
int client_read(CLIENT_SESSION *session, void *recvbuf, int recvbuf_len);
int client_send(CLIENT_SESSION *session, void *sendbuf, int sendlen);
void client_close(CLIENT_SESSION *session);

#endif //CLIENT_H_INCLUDED


