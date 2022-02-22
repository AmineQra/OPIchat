#include "xalloc.h"

#include <err.h>
#include <stdlib.h>

void *xmalloc(size_t size)
{
    void *res = malloc(size);
    if (!res)
        err(1, "Impossible to malloc");

    return res;
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *res = calloc(nmemb, size);
    if (!res)
        err(1, "Impossible to calloc");

    return res;
}

void *xrealloc(void *ptr, size_t size)
{
    void *res = realloc(ptr, size);
    if (!res)
        err(1, "Impossible to realloc");

    return res;
}
