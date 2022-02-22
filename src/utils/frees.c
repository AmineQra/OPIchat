#include "frees.h"

#include "xalloc.h"

void free_get_payload(struct command *cmd, struct package *pck, char *line)
{
    free_command(cmd);
    free_package(pck);
    xfree_null(line);
}

void *free_not_free(void *ptr)
{
    ptr = ptr;
    return NULL;
}