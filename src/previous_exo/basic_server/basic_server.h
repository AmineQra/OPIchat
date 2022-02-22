#ifndef BASIC_SERVER_H_
#define BASIC_SERVER_H_

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define DEFAULT_BUFFER_SIZE 2048

/**
 * \brief Iterate over the struct addrinfo elements to create and bind a socket
 *
 * \param addrinfo: struct addrinfo elements
 *
 * \return The created socket or exit with 1 if there is an error
 *
 * Try to create and connect a socket with every addrinfo element until it
 * succeeds
 *
 */
int create_and_bind(struct addrinfo *addrinfo);

/**
 * \brief Initialize the addrinfo struct and call create_and bind() and
 * listen(2)
 *
 * \param ip: IP address of the server
 * \param port: Port of the server
 *
 * \return The created socket
 *
 * Initialize the addrinfo struct needed by create_and_bind() before calling
 * it. When create_and_bind() returns a valid socket, set the socket to
 * listening and return it.
 */
int prepare_socket(const char *ip, const char *port);

/**
 * \brief Accept a new client
 *
 * \param socket: listening socket
 *
 * \return The new socket
 */
int accept_client(int socket);

/**
 * \brief Handle communication with a client
 *
 * \param client: client socket
 *
 * Receive a message from the client, and send back another message with the
 * same content.
 */
void communicate(int client_socket);

#endif /* BASIC_SERVER_H_ */