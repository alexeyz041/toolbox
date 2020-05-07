#include <stdio.h>       /* standard I/O routines                 */
#include <pthread.h>     /* pthread functions and data structures */


pthread_mutex_t request_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  got_request   = PTHREAD_COND_INITIALIZER;
int d;
int f;

pthread_mutex_t response_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  got_response   = PTHREAD_COND_INITIALIZER;
int r;


void *do_loop(void* data)
{
int *exit = (int *)data;
    pthread_mutex_lock(&request_mutex);
    for( ; *exit == 0; ) {
        int resp = 0;
        if(f) {
            printf("d = %d\n", d);
            f = 0;
            resp = 1;
        }
        pthread_mutex_unlock(&request_mutex);

        if(resp) {
            pthread_mutex_lock(&response_mutex);
            r = d+10;
            pthread_mutex_unlock(&response_mutex);
            pthread_cond_signal(&got_response);
        }

        pthread_mutex_lock(&request_mutex);
        pthread_cond_wait(&got_request, &request_mutex);
    }
    pthread_mutex_unlock(&request_mutex);
    pthread_exit(NULL);
}


int send_req(int n)
{
    pthread_mutex_lock(&request_mutex);
    d = n;
    f = 1;
    pthread_mutex_unlock(&request_mutex);
    pthread_cond_signal(&got_request);

    pthread_mutex_lock(&response_mutex);
    pthread_cond_wait(&got_response, &response_mutex);
    int rc = r;
    pthread_mutex_unlock(&response_mutex);
    return rc;
}


int main(int argc, char* argv[])
{
int thread_id;
pthread_t p_thread;
int exit = 0;
int i;

    thread_id = pthread_create(&p_thread, NULL, do_loop, (void*)&exit);
    for(i=0; i < 10; i++) {
        printf("rc(%d) = %d\n", i, send_req(i));
    }
    exit = 1;
    pthread_cond_signal(&got_request);
    pthread_join(p_thread, NULL);    
    return 0;
}
