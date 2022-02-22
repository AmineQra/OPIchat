#ifndef CLIENT_STRUCT_H
#define CLIENT_STRUCT_H

#include "list.h"

struct client_struct
{
    int *socket;
    char *login;
    struct l_list *rooms;
    struct l_list *owned_rooms;
};

/**
** \brief Init a struct client and return a pointer.
**
** \param socket the socket of the client. Used as an id the get the struct
** back.
** \return Return the struct that the user as to free later.
*/
struct client_struct *create_client_struct(int socket);

/**
** \brief Takes two void pointers and cast them into struct client pointer then
** compare there socket.
**
** \param ptr1 The first pointer that will be cast in a client_struct.
** \param ptr1 The second pointer that will be cast in a client_struct.
** \return Return 1 if the client's socket are the same, 0 otherwise.
*/
int compare_client_struct(void *ptr1, void *ptr2);

/**
** \brief Takes two void pointers and cast the first one into struct client
** pointer and the other into a int pointer then compare there socket.
**
** \param ptr_cli Pointer to a client struct.
** \param ptr_socket Pointer of researched socket.
** \return Return 1 if the socket is the same as the one in the struct, 0
** otherwise.
*/
int get_client_struct(void *ptr_cli, void *ptr_socket);

/**
** \brief Takes two void pointers and cast the first one into struct client
** pointer and the other into a char pointer then compare there login.
**
** \param ptr_cli Pointer to a client struct.
** \param ptr_login Pointer of researched login.
** \return Return 1 if the login is the same as the one in the struct, 0
*otherwise.
*/
int get_client_struct_by_login(void *ptr_cli, void *ptr_login);

/**
** \brief Add a copy of the room in joined rooms list if the room isn't already
** joined.
**
** \param cli Pointer to a client struct.
** \param rm_name Pointer to the name of the room to join.
** \param err Pointer to int that will be set to 1 if the room name already exit
*in the user list.
** \return Return the client struct.
*/
struct client_struct *add_room_client_struct(struct client_struct *cli,
                                             char *rm_name, int *err,
                                             int owned);

/**
** \brief Remove and free the room name in joined rooms list if the room hadn't
** already leaved it.
**
** \param cli Pointer to a client struct.
** \param rm_name Pointer to the name of the room to leave.
** \param err Pointer to int that will be set to 1 if the room name didn't exit
*in the user list.
** \return Return client struct.
*/
struct client_struct *remove_room_client_struct(struct client_struct *cli,
                                                char *rm_name, int *err,
                                                int owned);

/**
** \brief Free the old login of the client and set the new login with the param.
** the login is strcpy in the function.
**
** \param cli Pointer to a client struct.
** \param login Pointer to the new login login.
** \return Nothing (void).
*/
void update_login(struct client_struct *cli, char *login);

/**
** \brief Free the client struct and all it's elements
**
** \param cli Pointer the client struct cast and then to free.
** \return NULL.
*/
void *free_client_struct(void *ptr_cli);

#endif /* CLIENT_STRUCT_H */