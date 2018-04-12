
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>


#define NELEM(X)	(sizeof(X)/sizeof(X[0]))


int main(int argc,char *argv[])
{
wchar_t buf[10];

    printf("%d vs %d\n",sizeof(buf), NELEM(buf));
    swprintf(buf,sizeof(buf),L"Hello Craaaaaaash!\n");

    return 0;
}

