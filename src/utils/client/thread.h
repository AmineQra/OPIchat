#ifndef THREAD_H
#define THREAD_H

enum status_thread
{
    SHUTDOWN = 0,
    RUN = 1
};

struct thread_flag
{
    int socket;
    int *status;
    int *err;
    char **str;
    int (*fun_ptr)(int, int *, char **);
};

struct thread_flag *build_thread_flag(int socket, int *status, int *err,
                                      char **str,
                                      int (*fun_ptr)(int, int *, char **));

void *thread_routine(void *flag);

#endif /* THREAD_H */
