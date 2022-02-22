#include "epoll-server.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

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
                                   struct connection_t *connection)
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

    fprintf(stdout, "Client connected\n");
    return connection;
}

static char *concat(char *str, char *tmp)
{
    str = xrealloc(str, strlen(str) + strlen(tmp) + 1);

    str = strcat(str, tmp);
    if (!str)
        errx(1, "failed concat in buffer");

    return str;
}

static int sending_message(char *msg, struct connection_t *connection)
{
    struct connection_t *cursor = NULL;
    for (cursor = connection; cursor->next != NULL; cursor = cursor->next)
    {
        int sended =
            send(cursor->client_socket, msg, strlen(msg), MSG_NOSIGNAL);
        if (sended < 0)
        {
            fprintf(stderr, "Client disconnected\n");
            return 1;
        }
    }
    return 0;
}

static int receiving_message(int client_socket, char **str)
{
    char buff[DEFAULT_BUFFER_SIZE];
    *str = xcalloc(1, 20);
    int end = 0;

    do
    {
        end = recv(client_socket, &buff[0], DEFAULT_BUFFER_SIZE - 1,
                   MSG_NOSIGNAL);
        if (end <= 0)
            return 1;
        buff[end] = '\0';
        *str = concat(*str, buff);
    } while (!strchr(buff, '\n'));

    fprintf(stdout, "Received body: %s", *str);

    return 0;
}

static int communicate(int client_socket, struct connection_t *connection)
{
    char *str = NULL;
    int error = receiving_message(client_socket, &str);
    if (error)
        return 1;
    int sended = sending_message(str, connection);
    free(str);
    return sended;
}

static struct connection_t *client_disconnet(int client,
                                             struct connection_t *connection)
{
    fprintf(stdout, "Client disconnected\n");
    return remove_client(connection, client);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        errx(1, "invalid arg number");
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

    while (1)
    {
        int event_count = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);
        for (int i = 0; i < event_count; i++)
        {
            if (events[i].data.fd == socket)
                connection = accept_client(epoll_instance, socket, connection);
            else if (events[i].events & EPOLLIN)
            {
                if (communicate(events[i].data.fd, connection))
                {
                    connection =
                        client_disconnet(events[i].data.fd, connection);
                }
            }
        }
    }
    close(socket);
    return 0;
}
