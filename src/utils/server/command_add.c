#include "command_add.h"

#include <string.h>

#include "../package.h"
#include "../parsing.h"
#include "../tools.h"
#include "../xalloc.h"
#include "client_struct.h"
#include "connection.h"
#include "room.h"
#include "server_data.h"
#include "server_tools.h"

int handel_create_room(struct package *pck, struct server_data *serv_data,
                       int socket_client)
{
    int valid = 0;
    valid = char_matching_pattern(pck->payload, "^[a-zA-Z0-9]*$");
    if (!valid)
    {
        return request_out(socket_client, "\n3\nCREATE-ROOM\n",
                           dup_params(pck->params), "Bad room name\n");
    }
    int err = 0;
    err = already_in_list(serv_data->rooms, pck->payload, get_room);
    if (err)
    {
        return request_out(socket_client, "\n3\nCREATE-ROOM\n",
                           dup_params(pck->params), "Duplicate room name\n");
    }
    void *room_c = create_room(pck->payload, socket_client);
    serv_data->rooms = add_value(serv_data->rooms, room_c, get_room, &err);

    void *ptr_cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli = ptr_cli;

    add_room_client_struct(cli, pck->payload, &err, 1);

    return request_out(socket_client, "\n1\nCREATE-ROOM\n",
                       dup_params(pck->params), "Room created\n");
}

int handel_list_rooms(struct package *pck, struct server_data *serv_data,
                      int socket_client)
{
    struct l_list *list_ptr = serv_data->rooms;
    char *str = xcalloc(20, sizeof(char));
    while (list_ptr)
    {
        struct room *rm = list_ptr->value;
        str = concat(str, rm->room_name);
        str = concat(str, "\n");
        list_ptr = list_ptr->next_node;
    }
    // the previous pointer of str is free in the function
    return request_out(socket_client, "\n1\nLIST-ROOMS\n",
                       dup_params(pck->params), str);
}

int handel_join_room(struct package *pck, struct server_data *serv_data,
                     int socket_client)
{
    int found = 0;
    found = already_in_list(serv_data->rooms, pck->payload, get_room);
    if (!found)
    {
        return request_out(socket_client, "\n3\nJOIN-ROOM\n",
                           dup_params(pck->params), "Room not found\n");
    }

    void *ptr_cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli = ptr_cli;

    void *ptr_rm = get_node(serv_data->rooms, pck->payload, get_room);
    struct room *rm = ptr_rm;

    // add room to the joined rooms list in the client struct but not owned
    found = already_in_list(cli->rooms, pck->payload, xstrequals);
    if (!found)
    {
        add_room_client_struct(cli, pck->payload, &found, 0);
        add_socket_room(rm, socket_client, &found);
    }
    return request_out(socket_client, "\n1\nJOIN-ROOM\n",
                       dup_params(pck->params), "Room joined\n");
}

int handel_leave_room(struct package *pck, struct server_data *serv_data,
                      int socket_client)
{
    int found = 0;
    found = already_in_list(serv_data->rooms, pck->payload, get_room);
    if (!found)
    {
        return request_out(socket_client, "\n3\nLEAVE-ROOM\n",
                           dup_params(pck->params), "Room not found\n");
    }

    void *ptr_cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli = ptr_cli;

    void *ptr_rm = get_node(serv_data->rooms, pck->payload, get_room);
    struct room *rm = ptr_rm;

    // add room to the joined rooms list in the client struct but not owned
    found = already_in_list(cli->rooms, pck->payload, xstrequals);
    if (found)
    {
        remove_room_client_struct(cli, pck->payload, &found, 0);
        remove_socket_room(rm, socket_client, &found);
    }
    return request_out(socket_client, "\n1\nLEAVE-ROOM\n",
                       dup_params(pck->params), "Room left\n");
}

int handel_send_room(struct package *pck, struct server_data *serv_data,
                     int socket_client)
{
    struct param_tuple p_tuple = { .name_param1 = "Room",
                                   .name_param2 = NULL,
                                   .param1 = NULL,
                                   .param2 = NULL };
    struct param_tuple *ptr = get_params_values(&p_tuple, pck->params);
    p_tuple = *ptr;
    if (!p_tuple.param1)
    {
        return request_out(socket_client, "\n3\nSEND-ROOM\n",
                           dup_params(pck->params), "Missing parameter\n");
    }
    int found = 0;
    found = already_in_list(serv_data->rooms, p_tuple.param1, get_room);
    if (!found)
    {
        return request_out(socket_client, "\n3\nSEND-ROOM\n",
                           dup_params(pck->params), "Room not found\n");
    }

    void *ptr_rm = get_node(serv_data->rooms, p_tuple.param1, get_room);
    struct room *rm = ptr_rm;

    void *ptr_cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli_src = ptr_cli;

    struct l_list *ptr_sock = rm->list_socks;

    // build response
    p_tuple.name_param2 = "From";
    p_tuple.param2 = cli_src->login;
    char *payload = xcalloc(pck->payload_size + 1, sizeof(char) * 2);
    payload = strcpy(payload, pck->payload);
    char *str = char_for_package(pck->payload_size, "\n2\nSEND-ROOM\n",
                                 tuple_to_char(&p_tuple), payload);

    while (ptr_sock)
    {
        void *ptr_tmp_cli =
            get_node(serv_data->clients, ptr_sock->value, get_client_struct);
        struct client_struct *cli_tmp = ptr_tmp_cli;
        if (*(cli_tmp->socket) != socket_client)
            send_message(str, *(cli_tmp->socket));
        ptr_sock = ptr_sock->next_node;
    }

    // response to src
    return request_out(socket_client, "\n1\nSEND-ROOM\n",
                       dup_params(pck->params), "");
}

int handel_delete_room(struct package *pck, struct server_data *serv_data,
                       int socket_client)
{
    int found = 0;
    found = already_in_list(serv_data->rooms, pck->payload, get_room);
    if (!found)
    {
        return request_out(socket_client, "\n3\nDELETE-ROOM\n",
                           dup_params(pck->params), "Room not found\n");
    }

    void *ptr_cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);
    struct client_struct *cli = ptr_cli;

    void *ptr_rm = get_node(serv_data->rooms, pck->payload, get_room);
    struct room *rm = ptr_rm;

    if (rm->owner == socket_client)
    {
        struct l_list *ptr = rm->list_socks;
        while (ptr)
        {
            void *ptr_tmp_cli =
                get_node(serv_data->clients, ptr->value, get_client_struct);
            struct client_struct *cli_tmp = ptr_tmp_cli;
            remove_room_client_struct(cli_tmp, pck->payload, &found, 0);
            ptr = ptr->next_node;
        }
        remove_room_client_struct(cli, pck->payload, &found, 1);
        void **to_free = xcalloc(1, sizeof(void *));
        serv_data->rooms = remove_value(serv_data->rooms, pck->payload,
                                        get_room, &found, to_free);
        xfree(*to_free);
        xfree(to_free);
        return request_out(socket_client, "\n1\nDELETE-ROOM\n",
                           dup_params(pck->params), "Room deleted\n");
    }
    return request_out(socket_client, "\n3\nDELETE-ROOM\n",
                       dup_params(pck->params), "Unauthorized\n");
}

int handel_profile(struct package *pck, struct server_data *serv_data,
                   int socket_client)
{
    struct client_struct *cli =
        get_node(serv_data->clients, &socket_client, get_client_struct);

    char *str = xcalloc(20, sizeof(char));
    str = concat(str, "Username: ");
    str = concat(str, cli->login);
    str = concat(str, "\nIP: 127.0.0.1\nRooms:\n");

    struct l_list *cursor = serv_data->rooms;
    while (cursor)
    {
        struct room *rm = cursor->value;
        if (already_in_list(rm->list_socks, &socket_client, compare_sock))
        {
            str = concat(str, rm->room_name);
            str = concat(str, "\n");
        }
        cursor = cursor->next_node;
    }
    return request_out(socket_client, "\n1\nPROFILE\n", dup_params(pck->params),
                       str);
}