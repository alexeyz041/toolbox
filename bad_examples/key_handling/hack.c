
#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

static void* (*real_malloc)(size_t) = NULL;
static void (*real_free)(void *) = NULL;
static size_t size1;

static void mtrace_init(void)
{
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    if (!real_malloc) {
        fprintf(stderr, "Error in dlsym: %s\n", dlerror());
    }

    real_free = dlsym(RTLD_NEXT, "free");
    if (!real_free) {
        fprintf(stderr, "Error in dlsym: %s\n", dlerror());
    }
}



void *malloc(size_t size)
{
    if(!real_malloc) {
        mtrace_init();
    }

    void *p = NULL;
    fprintf(stderr, "malloc(%d) = ", (int)size);
    p = real_malloc(size);
    fprintf(stderr, "%p\n", p);
    size1 = size;
    return p;
}


void free(void *p)
{
unsigned char *u = (unsigned char *)p;

    for(int i=0; i < size1; i++) {
	fprintf(stderr,"%02x ",u[i]);
	if((i & 0x0f) == 0x0f) fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");

    real_free(p);
}
