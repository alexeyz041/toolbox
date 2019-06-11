
#include <stdio.h>
#include "hello.h"

void hello(context_t *ctx)
{
    printf("Hello, C World! %d %d\n",ctx->x,ctx->y);
    ctx->x++;

    snprintf(ctx->p, ctx->len, "hello");
}
