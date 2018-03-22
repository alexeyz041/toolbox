#include <stdlib.h>
#include <stdio.h>
#include "so/f.h"

int main(int argc, char **argv)
{
    int a = 13;
    printf ("%d %d\n", a, func(a) );
    return 0;
}
