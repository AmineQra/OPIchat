#ifndef FREES_H
#define FREES_H

#include "commands.h"
#include "package.h"

/**
** \brief Used in the client to do 3 free in one line.
**
** \param cmd The pointer to free.
** \param pck The pointer to free.
** \param line The pointer to free.
** \return Nothing (void).
*/
void free_get_payload(struct command *cmd, struct package *pck, char *line);

/**
** \brief Used for the generic list implementation if no free needed, it replace
** a normal ponter to a free function.
**
** \param ptr Does nothing with it.
** \return NULL.
*/
void *free_not_free(void *ptr);

#endif /* FREES_H */