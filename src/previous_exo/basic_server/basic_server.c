#include "basic_server.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create_and_bind(struct addrinfo *addrinfo)
{
    if (addrinfo == NULL)
    {
        exit(1);
    }
    for (struct addrinfo *curs = addrinfo; curs != NULL; curs = curs->ai_next)
    {
        int s = socket(curs->ai_family, curs->ai_socktype, curs->ai_protocol);
        int enable = 1;

        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            exit(1);

        if (s >= 0 && !bind(s, curs->ai_addr, curs->ai_addrlen))
        {
            return s;
        }
    }

    exit(1);
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
        exit(1);
    }
    int socker = create_and_bind(info_addr_serv);
    if (socker)
    {
        listen(socker, 20);
        freeaddrinfo(info_addr_serv);
        return socker;
    }
    exit(1);
}

int accept_client(int socket)
{
    int new_socket = accept(socket, NULL, 0);
    if (new_socket < 0)
        exit(1);
    fprintf(stdout, "Client connected\n");
    return new_socket;
}

static char *concat(char *str, char *tmp)
{
    str = realloc(str, strlen(str) + strlen(tmp) + 1);
    if (!str)
        exit(1);

    str = strcat(str, tmp);
    if (!str)
        exit(1);

    return str;
}

static int sending_message(int client_socket, char *msg)
{
    if (send(client_socket, msg, strlen(msg), MSG_NOSIGNAL) < 0)
    {
        fprintf(stdout, "Client disconnected\n");
        return 1;
    }
    return 0;
}

static int receiving_message(int client_socket)
{
    char buff[DEFAULT_BUFFER_SIZE];
    char *str = calloc(1, 20);
    int end = 0;

    if (!str)
        exit(1);

    do
    {
        end = recv(client_socket, &buff[0], DEFAULT_BUFFER_SIZE - 1,
                   MSG_NOSIGNAL);
        if (end <= 0)
            return 1;
        buff[end] = '\0';
        str = concat(str, buff);
    } while (!strchr(buff, '\n'));

    fprintf(stdout, "Received body: %s", str);

    int sended = sending_message(client_socket, str);
    free(str);
    if (sended)
        return 1;
    return 0;
}

void communicate(int client_socket)
{
    while (1)
    {
        if (receiving_message(client_socket))
        {
            fprintf(stdout, "Client disconnected\n");
            close(client_socket);
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        return 1;
    int socket = prepare_socket(argv[1], argv[2]);
    if (socket < 0)
        return 1;
    while (1)
    {
        int new_client = accept_client(socket);
        communicate(new_client);
    }
    close(socket);
    return 0;
}