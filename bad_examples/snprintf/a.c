
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc,char *argv[])
{
uint32_t a = 32;
uint64_t b = 64;
char str[64] = {0};

    printf("%lu %lu\n",sizeof(a),sizeof(b));
    snprintf(str,sizeof(str),"%08x %08lx\n",a,b);
    printf("%s",str);

    return 0;
}

