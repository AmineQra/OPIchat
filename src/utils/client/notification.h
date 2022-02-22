#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "../package.h"

/**
** \brief Give the struct command corespondinf to the name.
**
** \param command_name The pointer of the command name.
** \return The pointer to the struct command corresponding to the name, null if
*the name doesn't exit.
*/
void notif_send_dm_brdc(struct package *pck);

/**
** \brief Free the command struct.
**
** \param cmd The pointer of the command.
** \return NULL.
*/
void notif_send_room(struct package *pck);

#endif /* NOTIFICATION_H */