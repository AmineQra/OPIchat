#include "client_struct.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../frees.h"
#include "../tools.h"
#include "../xalloc.h"
#include "list.h"

struct client_struct *create_client_struct(int socket)
{
    struct client_struct *cli = xcalloc(1, sizeof(struct client_struct));
    char *login = xcalloc(strlen("<Anonymous>"), sizeof(char) * 2);
    login = strcpy(login, "<Anonymous>");
    cli->socket = xcalloc(1, sizeof(int));
    *(cli->socket) = socket;
    cli->login = login;
    cli->rooms = NULL;
    cli->owned_rooms = NULL;
    return cli;
}

int compare_client_struct(void *ptr1, void *ptr2)
{
    struct client_struct *cli1 = ptr1;
    struct client_struct *cli2 = ptr2;
    return *(cli1->socket) == *(cli2->socket);
}

int get_client_struct(void *ptr_cli, void *ptr_socket)
{
    struct client_struct *cli = ptr_cli;
    int *socket = ptr_socket;
    return *(cli->socket) == *socket;
}

int get_client_struct_by_login(void *ptr_cli, void *ptr_login)
{
    struct client_struct *cli = ptr_cli;
    char *login = ptr_login;
    return strequals(cli->login, login);
}

struct client_struct *add_room_client_struct(struct client_struct *cli,
                                             char *rm_name, int *err, int owned)
{
    char *name = xcalloc(strlen(rm_name), sizeof(char) * 2);
    name = strcpy(name, rm_name);
    if (owned)
        cli->owned_rooms = add_value(cli->owned_rooms, name, xstrequals, err);
    else
        cli->rooms = add_value(cli->rooms, name, xstrequals, err);
    return cli;
}

struct client_struct *remove_room_client_struct(struct client_struct *cli,
                                                char *rm_name, int *err,
                                                int owned)
{
    // check remove owner ?
    void **ptr = xcalloc(1, sizeof(void *));
    if (owned)
        cli->owned_rooms =
            remove_value(cli->owned_rooms, rm_name, xstrequals, err, ptr);
    else
        cli->rooms = remove_value(cli->rooms, rm_name, xstrequals, err, ptr);
    char *str = *ptr;
    str = xfree(str);
    ptr = xfree(ptr);
    return cli;
}

void update_login(struct client_struct *cli, char *login)
{
    cli->login = xfree(cli->login);
    char *log = xcalloc(strlen(login), sizeof(char) * 2);
    log = strcpy(log, login);
    cli->login = log;
}

void *free_client_struct(void *ptr_cli)
{
    struct client_struct *cli = ptr_cli;
    cli->login = xfree(cli->login);
    cli->socket = xfree(cli->socket);
    if (cli->rooms)
        free_list(cli->rooms, xfree);
    free(cli);
    return NULL;
}