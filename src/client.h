#ifndef CLIENT_H
#define CLIENT_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define DEFAULT_BUFFER_SIZE 2048

/**
 * \brief Iterate over the struct addrinfo elements to connect to the server
 *
 * \param addrinfo: struct addrinfo elements
 *
 * \return The created socket or exit with 1 if there is an error
 *
 * Try to create and connect a socket with every addrinfo element until it
 * succeeds
 *
 */
int create_and_connect(struct addrinfo *addrinfo);

/**
 * \brief Initialize the addrinfo struct and call create_and_connect()
 *
 * \param ip: IP address of the server
 * \param port: Port of the server
 *
 * \return The created socket
 */
int prepare_socket(const char *ip, const char *port);

#endif /* CLIENT_H */