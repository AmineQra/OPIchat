#include "room.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../frees.h"
#include "../tools.h"
#include "../xalloc.h"
#include "list.h"
#include "server_tools.h"

struct room *create_room(char *room_name, int socket)
{
    struct room *rm = xcalloc(1, sizeof(struct room));
    socket = socket;

    char *cpy_room_name = xcalloc(strlen(room_name), sizeof(char) * 2);
    cpy_room_name = strcpy(cpy_room_name, room_name);
    rm->room_name = cpy_room_name;
    rm->owner = socket;

    rm->list_socks = NULL;
    return rm;
}

int compare_room(void *ptr1, void *ptr2)
{
    struct room *r1 = ptr1;
    struct room *r2 = ptr2;
    return strequals(r1->room_name, r2->room_name);
}

int get_room(void *ptr_rm, void *ptr_name)
{
    struct room *rm = ptr_rm;
    char *name = ptr_name;
    return strequals(rm->room_name, name);
}

struct room *add_socket_room(struct room *rm, int socket, int *err)
{
    void *p_sock = xcalloc(1, sizeof(int));
    int *tmp = p_sock;
    *tmp = socket;
    rm->list_socks = add_value(rm->list_socks, p_sock, compare_sock, err);
    return rm;
}

struct room *remove_socket_room(struct room *rm, int socket, int *err)
{
    // check remove owner ?
    void **ptr = xcalloc(1, sizeof(void *));
    rm->list_socks =
        remove_value(rm->list_socks, &socket, compare_sock, err, ptr);
    // for the moment no free on the pointed zone (int)
    ptr = xfree(ptr);
    return rm;
}

void *free_room(void *ptr_room)
{
    struct room *rm = ptr_room;
    xfree(rm->room_name);
    if (rm->list_socks)
        free_list(rm->list_socks, xfree);
    free(rm);
    return NULL;
}