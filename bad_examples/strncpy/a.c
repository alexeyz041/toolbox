
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char *argv[])
{
    char str[16] = {0};

    strncpy(str,"012345678890123456789",sizeof(str));

    for(int i=0; i < sizeof(str); i++) {
	printf("%02x %c  ", str[i], (str[i] >= ' ') ? str[i] : '.');
    }
    printf("\n");

    printf("%s\n",str);

    return 0;
}

