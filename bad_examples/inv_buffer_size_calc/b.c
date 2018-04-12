
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char *argv[])
{
    const char *s1 = "Hello ";
    const char *s2 = "World!";
    int len = strlen(s1) + strlen(s2);
    char *buf = (char *)malloc(len);
    if(buf) {
	strcpy(buf,s1);
	strcat(buf,s2);
	printf("len = %d\n",strlen(buf));
	free(buf);
    }
    return 0;
}

