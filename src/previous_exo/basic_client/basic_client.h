#ifndef BASIC_CLIENT_H_
#define BASIC_CLIENT_H_

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

/**
 * \brief Handle communication with the server
 *
 * \param server_socket: server socket
 *
 * Read user message from STDIN and send it to the server
 *
 * Step 2: This function sends small messages to the server
 *
 * Step 3: In addition to step 2 behavior, this function receives the server
 * response on small messages
 *
 * Step 4: The function must now handle long messages
 */
void communicate(int server_socket);

#endif /* BASIC_CLIENT_H_ */