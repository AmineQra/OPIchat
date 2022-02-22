#ifndef SERVER_TOOLS_H
#define SERVER_TOOLS_H

#include "../parsing.h"
#include "connection.h"
#include "list.h"

int bad_request(int client_socket);

int request_out(int sock, char *mid_msg, char *params, char *payload);

char *dup_params(char *to_dup);

int compare_sock(void *ptr_i1, void *ptr_i2);

#endif /* SERVER_TOOLS_H */