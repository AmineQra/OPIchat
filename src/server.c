#include "server.h"

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "utils/package.h"
#include "utils/server/client_struct.h"
#include "utils/server/command_add.h"
#include "utils/server/command_basic.h"
#include "utils/server/connection.h"
#include "utils/server/list.h"
#include "utils/server/room.h"
#include "utils/server/server_data.h"
#include "utils/server/server_tools.h"
#include "utils/tools.h"
#include "utils/xalloc.h"

int create_and_bind(struct addrinfo *addrinfo)
{
    if (addrinfo == NULL)
    {
        errx(1, "error addrinfo is nul in create and bind");
    }
    for (struct addrinfo *curs = addrinfo; curs != NULL; curs = curs->ai_next)
    {
        int s = socket(curs->ai_family, curs->ai_socktype, curs->ai_protocol);
        int enable = 1;

        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            errx(1, "error in create and bind");

        if (s >= 0 && !bind(s, curs->ai_addr, curs->ai_addrlen))
        {
            return s;
        }
    }

    errx(1, "failed create and bind server socket");
}

int prepare_socket(const char *ip, const char *port)
{
    struct addrinfo info_addr_cli;
    struct addrinfo *info_addr_serv;

    memset(&info_addr_cli, 0, sizeof(info_addr_cli));
    info_addr_cli.ai_family = AF_UNSPEC; // IPv4 or IPv6
    info_addr_cli.ai_socktype = SOCK_STREAM; // TCP stream sockets

    int status = getaddrinfo(ip, port, &info_addr_cli, &info_addr_serv);
    if (status != 0)
    {
        errx(1, "failed getaddrinfo server");
    }
    int socker = create_and_bind(info_addr_serv);
    if (socker)
    {
        listen(socker, 20);
        freeaddrinfo(info_addr_serv);
        return socker;
    }
    errx(1, "error prepare server socket");
}

struct connection_t *accept_client(int epoll_instance, int socket,
                                   struct connection_t *connection,
                                   struct server_data *serv_data)
{
    int new_socket = accept(socket, NULL, 0);
    if (new_socket < 0)
        errx(1, "failed socket client");

    connection = add_client(connection, new_socket);

    struct epoll_event cli_ev;
    cli_ev.events = EPOLLIN;
    cli_ev.data.fd = new_socket;

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, new_socket, &cli_ev) < 0)
        errx(1, "failed epoll ctp client");
    struct client_struct *cli = create_client_struct(new_socket);
    int err = 0;
    serv_data->clients =
        add_value(serv_data->clients, cli, compare_client_struct, &err);
    fprintf(stdout, "Client connected\n");
    return connection;
}

static void leave_rooms(struct client_struct *cli,
                        struct server_data *serv_data, int client)
{
    struct l_list *ptr_rooms = cli->rooms;
    int ptdr = 0;
    while (ptr_rooms)
    {
        char *rm_name = ptr_rooms->value;
        printf("je me casse: %s_\n", rm_name);
        struct room *rm = get_node(serv_data->rooms, rm_name, get_room);
        remove_socket_room(rm, client, &ptdr);
        ptr_rooms = ptr_rooms->next_node;
    }
}

static void delete_rooms(struct client_struct *cli,
                         struct server_data *serv_data)
{
    struct l_list *ptr_rooms = cli->owned_rooms;
    int sock = *(cli->socket);
    int found = 0;
    while (ptr_rooms)
    {
        char *rm_name = ptr_rooms->value;
        struct room *rm = get_node(serv_data->rooms, rm_name, get_room);
        struct l_list *ptr = rm->list_socks;

        while (ptr)
        {
            struct client_struct *cli_tmp =
                get_node(serv_data->clients, ptr->value, get_client_struct);
            if (*(cli_tmp->socket) != sock)
                remove_room_client_struct(cli_tmp, rm->room_name, &found, 0);
            ptr = ptr->next_node;
        }
        void **to_free = xcalloc(1, sizeof(void *));
        serv_data->rooms = remove_value(serv_data->rooms, rm->room_name,
                                        get_room, &found, to_free);
        free(*to_free);
        free(to_free);

        ptr_rooms = ptr_rooms->next_node;
    }
}

struct connection_t *client_disconnet(int client,
                                      struct connection_t *connection,
                                      struct server_data *serv_data)
{
    fprintf(stdout, "Client disconnected\n");
    void *ptr_cli = get_node(serv_data->clients, &client, get_client_struct);
    struct client_struct *cli = ptr_cli;
    leave_rooms(cli, serv_data, client);
    delete_rooms(cli, serv_data);
    return remove_client(connection, serv_data, client);
}

int handel_command(struct package *pck, struct connection_t *connection,
                   struct server_data *serv_data, int client_socket)
{
    char *str = package_to_char(pck);
    fprintf(stdout, "< Request in:\n%s\n\n", str);
    str = xfree(str);

    if (strequals(pck->cmd_name, "PING"))
    {
        return handel_ping(pck, client_socket);
    }
    else if (strequals(pck->cmd_name, "LOGIN"))
    {
        return handel_login(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "LIST-USERS"))
    {
        return handel_list_users(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "SEND-DM"))
    {
        return handel_send_dm(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "BROADCAST"))
    {
        return handel_broadcast(pck, connection, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "CREATE-ROOM"))
    {
        return handel_create_room(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "LIST-ROOMS"))
    {
        return handel_list_rooms(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "JOIN-ROOM"))
    {
        return handel_join_room(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "LEAVE-ROOM"))
    {
        return handel_leave_room(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "SEND-ROOM"))
    {
        return handel_send_room(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "DELETE-ROOM"))
    {
        return handel_delete_room(pck, serv_data, client_socket);
    }
    else if (strequals(pck->cmd_name, "PROFILE"))
    {
        return handel_profile(pck, serv_data, client_socket);
    }
    return 1;
}

static int parse_request(char *request, struct connection_t *connection,
                         struct server_data *serv_data, int client_socket)
{
    int err = 0;
    char *ptr = request;
    struct package *pck = char_to_package(request, &err);
    ptr = xfree(ptr);
    if (err)
    {
        return bad_request(client_socket);
    }
    err = handel_command(pck, connection, serv_data, client_socket);
    pck = free_package(pck);
    return err;
}

int receiving_message(int client_socket, struct connection_t *connection,
                      struct server_data *serv_data)
{
    char buff[DEFAULT_BUFFER_SIZE];
    char *str = xcalloc(1, 20);
    int end = 0;

    do
    {
        end = recv(client_socket, &buff[0], DEFAULT_BUFFER_SIZE - 1,
                   MSG_NOSIGNAL);
        if (end <= 0)
            return 1;
        buff[end] = '\0';
        str = concat(str, buff);
    } while (!strchr(buff, '\n'));

    fprintf(stdout, "Client data\n");

    return parse_request(str, connection, serv_data, client_socket);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        errx(1, "usage: /student <ip> <port>");
    int socket = prepare_socket(argv[1], argv[2]);
    if (socket < 0)
        errx(1, "failed init server socket");

    // Create an epoll instance
    int epoll_instance = epoll_create1(0);

    struct epoll_event event;
    event.data.fd = socket;
    event.events = EPOLLIN;

    struct connection_t *connection = add_client(NULL, socket);

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, socket, &event))
        errx(1, "faild epoll ctl server");

    struct epoll_event events[MAX_EVENTS];

    struct server_data *serv_data = xcalloc(1, sizeof(struct server_data));
    serv_data->rooms = NULL;
    serv_data->clients = NULL;

    while (1)
    {
        int event_count = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);
        for (int i = 0; i < event_count; i++)
        {
            if (events[i].data.fd == socket)
                connection = accept_client(epoll_instance, socket, connection,
                                           serv_data);
            else if (events[i].events & EPOLLIN)
            {
                if (receiving_message(events[i].data.fd, connection, serv_data))
                {
                    connection = client_disconnet(events[i].data.fd, connection,
                                                  serv_data);
                }
            }
        }
    }
    close(socket);
    return 0;
}