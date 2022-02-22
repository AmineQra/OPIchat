#include "command_basic.h"

#include <stdio.h>
#include <string.h>

#include "../package.h"
#include "../parsing.h"
#include "../tools.h"
#include "../xalloc.h"
#include "client_struct.h"
#include "connection.h"
#include "list.h"
#include "server_data.h"

int handel_ping(struct package *pck, int socket_client)
{
    return request_out(socket_client, "\n1\nPING\n", dup_params(pck->params),
                       "PONG\n");
}

int handel_login(struct package *pck, struct server_data *serv_data,
                 int socket_client)
{
    int valid = 0;
    valid = char_matching_pattern(pck->payload, "^[a-zA-Z0-9]*$");
    if (!valid)
    {
        return request_out(socket_client, "\n3\nLOGIN\n",
                           dup_params(pck->params), "Bad username\n");
    }
    int err = 0;
    err = already_in_list(serv_data->clients, pck->payload,
                          get_client_struct_by_login);
    if (err == 1)
    {
        return request_out(socket_client, "\n3\nLOGIN\n",
                           dup_params(pck->params), "Duplicate username\n");
    }
    void *ptr_cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli = ptr_cli;
    // strcpy dans le update du paylod sinon free donc perdu
    update_login(cli, pck->payload);

    return request_out(socket_client, "\n1\nLOGIN\n", dup_params(pck->params),
                       "Logged in\n");
}

int handel_list_users(struct package *pck, struct server_data *serv_data,
                      int socket_client)
{
    struct l_list *list_ptr = serv_data->clients;
    char *str = xcalloc(20, sizeof(char));
    while (list_ptr)
    {
        struct client_struct *cli = list_ptr->value;
        if (!strequals(cli->login, "<Anonymous>"))
        {
            str = concat(str, cli->login);
            str = concat(str, "\n");
        }
        list_ptr = list_ptr->next_node;
    }
    // the previous pointer of str is free in the function
    return request_out(socket_client, "\n1\nLIST-USERS\n",
                       dup_params(pck->params), str);
}

int handel_send_dm(struct package *pck, struct server_data *serv_data,
                   int socket_client)
{
    struct param_tuple p_tuple = { .name_param1 = "User",
                                   .name_param2 = NULL,
                                   .param1 = NULL,
                                   .param2 = NULL };
    struct param_tuple *ptr = get_params_values(&p_tuple, pck->params);
    p_tuple = *ptr;
    if (!p_tuple.param1)
    {
        return request_out(socket_client, "\n3\nSEND-DM\n",
                           dup_params(pck->params), "Missing parameter\n");
    }
    void *ptr_cli = get_node(serv_data->clients, p_tuple.param1,
                             get_client_struct_by_login);
    if (!ptr_cli)
    {
        return request_out(socket_client, "\n3\nSEND-DM\n",
                           dup_params(pck->params), "User not found\n");
    }

    // get both client struct
    struct client_struct *cli_dest = ptr_cli;
    ptr_cli = get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli_src = ptr_cli;

    // update param struct
    p_tuple.name_param2 = "From";
    p_tuple.param2 = cli_src->login;

    // response to dest
    request_out(*(cli_dest->socket), "\n2\nSEND-DM\n", tuple_to_char(&p_tuple),
                pck->payload);

    // response to src
    return request_out(*(cli_src->socket), "\n1\nSEND-DM\n",
                       dup_params(pck->params), "");
}

int handel_broadcast(struct package *pck, struct connection_t *connection,
                     struct server_data *serv_data, int socket_client)
{
    // get client struct
    void *ptr_cli = NULL;
    ptr_cli = get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli = ptr_cli;

    struct param_tuple p_tuple = { .name_param1 = "From",
                                   .name_param2 = NULL,
                                   .param1 = cli->login,
                                   .param2 = NULL };

    // build response
    char *payload = xcalloc(pck->payload_size + 1, sizeof(char) * 2);
    payload = strcpy(payload, pck->payload);
    char *str = char_for_package(pck->payload_size, "\n2\nBROADCAST\n",
                                 tuple_to_char(&p_tuple), payload);

    // response to dest
    struct connection_t *cursor = connection;

    while (cursor)
    {
        if (cursor->client_socket != socket_client)
            send_message(str, cursor->client_socket);
        cursor = cursor->next;
    }
    fprintf(stdout, "< Request out:\n%s\n", str);
    str = xfree(str);

    // response to src
    fprintf(stdout, "< Request out:\n%s\n", "0\n1\nBROADCAST\n\n");
    return send_message("0\n1\nBROADCAST\n\n", socket_client);
}