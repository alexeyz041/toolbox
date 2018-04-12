
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


void decrypt_key(unsigned char *key,int length)
{
    for(int i=0; i < length; i++) {
	key[i] = rand();
    }
}


int main(int argc, char *argv[])
{
    srand(time(NULL));
    char *buf = (char *)malloc(16);
    if(buf) {
	decrypt_key(buf,16);

	// do something secret here

	memset(buf,0,16);
	free(buf);
    }

    return 0;
}
