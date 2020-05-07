#include <stdio.h>
#include <pthread.h>
#include <errno.h>


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
    
    for( ; *exit == 0; ) {
        pthread_mutex_lock(&request_mutex);
        f = 1;
        pthread_cond_wait(&got_request, &request_mutex);
        printf("d = %d\n", d);
        pthread_mutex_unlock(&request_mutex);

//        if(resp) {
            pthread_mutex_lock(&response_mutex);
            r = d+10;
            pthread_mutex_unlock(&response_mutex);
            pthread_cond_signal(&got_response);
//        }
    }
    pthread_mutex_unlock(&request_mutex);
    pthread_exit(NULL);
}


int send_req(int n)
{
    pthread_mutex_lock(&request_mutex);
    if(!f) {
        pthread_mutex_unlock(&request_mutex);
        return -1;
    }
    d = n;
    pthread_mutex_unlock(&request_mutex);
    pthread_cond_signal(&got_request);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;

    pthread_mutex_lock(&response_mutex);
    int res = pthread_cond_timedwait(&got_response, &response_mutex, &ts);
    int rc = r;
    pthread_mutex_unlock(&response_mutex);

    if(res) {
       printf("timedwaid rc=%d %s\n", res, (res == ETIMEDOUT) ? "timeout" : "");
       return -1;
    }

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
        int a = send_req(i);
        if(a == -1) usleep(1);
        printf("rc(%d) = %d\n", i, a);
    }
    exit = 1;
    send_req(14);
    pthread_join(p_thread, NULL);    
    return 0;
}
