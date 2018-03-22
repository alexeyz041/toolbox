#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>


int main(int argc, char **argv)
{
    void *handle;
    int (*func)(int);
    char *error;
    int a = 13;

    handle = dlopen ("so/libf.so", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }

    func = dlsym(handle, "func");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }

    printf ("%d %d\n", a, (*func)(a) );
    dlclose(handle);
    return 0;
}
