// ./auth alice 1234		intended correct user name/password
// ./auth alice qwertyuiopa 	buffer overflow cause unintended access granted

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char buf[16];
    int auth;
    int hash;
} ctx_t;


int auth(char *username,char *password)
{
ctx_t c;
    memset(&c,0,sizeof(c));

    strcpy(c.buf,username);
    strcat(c.buf,"|");
    strcat(c.buf,password);

    c.hash = 13;
    for(int i=0; c.buf[i]; i++) {
	c.hash += c.buf[i];
    }

    if(c.hash == 849) c.auth = 1;
    return c.auth;
}


int main(int argc,char *argv[])
{
    if(argc < 3) {
	printf("auth user pwd\n");
	return 1;
    }

    if(auth(argv[1],argv[2])) {
	printf("access granted\n");
    } else {
	printf("access denied\n");
    }
    return 0;
}

