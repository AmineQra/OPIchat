#include "xalloc.h"

#include <err.h>
#include <stdlib.h>

void *xmalloc(size_t size)
{
    void *res = malloc(size);
    if (!res)
        err(1, "[XALLOC] [MALLOC] Impossible to malloc");

    return res;
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *res = calloc(nmemb, size);
    if (!res)
        err(1, "[XALLOC] [CALLOC] Impossible to calloc");

    return res;
}

void *xrealloc(void *ptr, size_t size)
{
    void *res = realloc(ptr, size);
    if (!res)
        err(1, "[XALLOC] [RALLOC] Impossible to realloc");

    return res;
}

void *xfree(void *ptr)
{
    free(ptr);
    return NULL;
}

void *xfree_null(void *ptr)
{
    if (ptr)
        free(ptr);
    return NULL;
}