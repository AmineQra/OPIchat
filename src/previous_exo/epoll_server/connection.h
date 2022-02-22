#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <sys/types.h>

/**
 * \brief Contain all the information about all clients (linked list)
 */
struct connection_t
{
    int client_socket; /**< socket fd of the client */

    char *buffer; /**< buffer containing all the data received by this client */

    ssize_t nb_read; /**< number of bytes read (also size of the buffer) */

    struct connection_t *next; /**< next connection_t for another client */
};

/**
 * \brief Add a new client connection_t to the linked list connection
 *
 * \param connection: the connection_t linked list with all the clients
 *
 * \param client_socket: the client socket fd to add
 *
 * \return The connection_t linked list with the element added
 *
 * Add the new connection_t element with a head insert
 */
struct connection_t *add_client(struct connection_t *connection,
                                int client_socket);

/**
 * \brief Remove the client connection_t from the linked list connection
 *
 * \param connection: the connection_t linked list with all the clients
 *
 * \param client_socket: the client socket fd to remove
 *
 * \return The connection_t linked list with element removed
 *
 * Iterate over the linked list to find the right connection_t and remove it
 */
struct connection_t *remove_client(struct connection_t *connection,
                                   int client_socket);
/**
 * \brief Find the connection_t element where the socket is equal to client sock
 *
 * \param connection: the connection_t linked list with all the clients
 *
 * \param client_socket: the client socket to find
 *
 * \return The connection_t element of the specific client
 *
 * Iterate over the linked list until it finds the connection_t. If the client
 * is not in the linked list returns NULL
 */
struct connection_t *find_client(struct connection_t *connection,
                                 int client_socket);

#endif /* CONNECTION_H */