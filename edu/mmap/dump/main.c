#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc,char *argv[])
{
unsigned long long base = strtoull(argv[1],NULL,16);
size_t len = 1024;
int memfd;

    printf("dev_base = 0x%0llx\n", base);

    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    unsigned char *p = mmap(0, len, PROT_READ, MAP_SHARED|MAP_FIXED, memfd, base);
    if (p == MAP_FAILED) {
	printf("mmap failed\n");
	exit(1);
    }
    printf("mapped_base = %p\n", p);

    for(int i=0; i < 16; i++) {
	printf("%02x ",p[i]);
    }
    printf("\n");

    munmap(p, len);
    close(memfd);
    return 0;
}
