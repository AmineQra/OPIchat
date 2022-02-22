#ifndef SERVER_DATA_H
#define SERVER_DATA_H

#include "list.h"

/**
** \brief All the informations abouts clients connected an all the rooms
** created.
**
** \param rooms Linked list that contains all the rooms created (the struct
** not only the name).
** \param next_node Linked list that contains all the clients connected
** (the client_struct not only the socket).
*/
struct server_data
{
    struct l_list *rooms;
    struct l_list *clients;
};

#endif /* SERVER_DATA_H */