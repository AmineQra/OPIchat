#ifndef COMMAND_BASIC_H
#define COMMAND_BASIC_H

#include "../package.h"
#include "connection.h"
#include "server_tools.h"

int handel_ping(struct package *pck, int socket_client);

int handel_login(struct package *pck, struct server_data *serv_data,
                 int socket_client);

int handel_list_users(struct package *pck, struct server_data *serv_data,
                      int socket_client);

int handel_send_dm(struct package *pck, struct server_data *serv_data,
                   int socket_client);

int handel_broadcast(struct package *pck, struct connection_t *connection,
                     struct server_data *serv_data, int socket_client);

#endif /* COMMAND_BASIC_H */