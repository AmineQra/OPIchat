#ifndef ROOM_H
#define ROOM_H

#include "list.h"

/**
** \brief Contains all intels about a room.
**
** \param room_name The name of the room.
** \param owner The client's socket that have created the room.
** \param list_socks The linked list that contains all the client's socket
** that are connected to the room.
*/
struct room
{
    char *room_name;
    int owner;
    struct l_list *list_socks;
};

/**
** \brief Init a struct Rooms and return a pointer. the user must free the
** pointer later
**
** \param room_name The name of the room.
** \param socket The owner's socket.
** \return Return the struct pointer.
*/
struct room *create_room(char *room_name, int socket);

/**
** \brief Takes two void pointers and cast them into struct room pointer then
** compare there name.
**
** \param ptr1 The first pointer that will be cast in a room struct.
** \param ptr1 The second pointer that will be cast in a room struct.
** \return Return 1 if the room's name are the same, 0 otherwise.
*/
int compare_room(void *ptr1, void *ptr2);

/**
** \brief Takes two void pointers and cast the first one into struct room
** pointer and the other into a char pointer then compare there name.
**
** \param ptr_rm Pointer to a room struct.
** \param ptr_name Pointer of researched room's name.
** \return Return 1 if the name is the same as the one in the struct, 0
*otherwise.
*/
int get_room(void *ptr_rm, void *ptr_name);

/**
** \brief Add the socket in joined client's socket list if the client asn't
** already joined.
**
** \param rm Pointer to a room struct.
** \param socket Pointer to the client's socket to add.
** \param err Pointer to int that will be set to 1 if the cclient's socket
** already exit in the user list.
** \return Return the room struct.
*/
struct room *add_socket_room(struct room *rm, int socket, int *err);

/**
** \brief Remove the client's socket in joined client's socket list if the
** client hadn't already leaved it.
**
** \param rm Pointer to the room struct.
** \param socket Pointer to the client's socket to remove.
** \param err Pointer to int that will be set to 1 if the client's socket didn't
** exit in the user list.
** \return Return room struct.
*/
struct room *remove_socket_room(struct room *rm, int socket, int *err);

/**
** \brief Free the room struct and all it's elements
**
** \param ptr_room Pointer the room struct cast and then to free.
** \return NULL.
*/
void *free_room(void *ptr_room);

#endif /* ROOM_H */