#ifndef COMMAND_ADD_H
#define COMMAND_ADD_H

#include "../package.h"
#include "connection.h"
#include "server_data.h"

int handel_create_room(struct package *pck, struct server_data *serv_data,
                       int socket_client);

int handel_list_rooms(struct package *pck, struct server_data *serv_data,
                      int socket_client);

int handel_join_room(struct package *pck, struct server_data *serv_data,
                     int socket_client);

int handel_leave_room(struct package *pck, struct server_data *serv_data,
                      int socket_client);

int handel_send_room(struct package *pck, struct server_data *serv_data,
                     int socket_client);

int handel_delete_room(struct package *pck, struct server_data *serv_data,
                       int socket_client);

int handel_profile(struct package *pck, struct server_data *serv_data,
                   int socket_client);

#endif /* COMMAND_ADD_H */