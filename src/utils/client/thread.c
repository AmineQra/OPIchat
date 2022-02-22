#include "thread.h"

#include <stdio.h>

#include "../xalloc.h"

struct thread_flag *build_thread_flag(int socket, int *status, int *err,
                                      char **str,
                                      int (*fun_ptr)(int, int *, char **))
{
    struct thread_flag *f = xcalloc(1, sizeof(struct thread_flag));
    f->socket = socket;
    f->status = status;
    f->err = err;
    f->str = str;
    f->fun_ptr = fun_ptr;
    return f;
}

void *thread_routine(void *flag)
{
    struct thread_flag *f = flag;
    int *status = f->status;
    while (*status == RUN)
    {
        if (f->fun_ptr(f->socket, f->err, f->str) == SHUTDOWN)
            *status = SHUTDOWN;
    }
    return f->err;
}