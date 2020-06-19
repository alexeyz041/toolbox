
#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

typedef struct {
    int port;
    void (*wcb)(void *, void *, int, int);
    void *ctx;
} server_arg_t;


typedef struct {
    int sock;
    void (*wcb)(void *, void *, int, int);
    void *ctx;
} worker_arg_t;


void *server_session(void *p);

int server_send(int s, uint8_t *sendbuf, int sendlen);

void *server(void *p);

int start_server(int port, void (*wcb)(void *, void *, int, int), void *ctx);

#endif //SERVER_H_INCLUDED
