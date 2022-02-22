#include "basic_client.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create_and_connect(struct addrinfo *addrinfo)
{
    if (addrinfo == NULL)
    {
        exit(1);
    }
    for (struct addrinfo *curs = addrinfo; curs != NULL; curs = curs->ai_next)
    {
        int s = socket(curs->ai_family, curs->ai_socktype, curs->ai_protocol);
        if (s >= 0 && !connect(s, curs->ai_addr, curs->ai_addrlen))
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
    int socker = create_and_connect(info_addr_serv);
    if (socker)
    {
        freeaddrinfo(info_addr_serv);
        return socker;
    }
    exit(1);
}

char *concat(char *str, char *tmp)
{
    str = realloc(str, strlen(str) + strlen(tmp) + 1);
    if (!str)
    {
        exit(1);
    }
    str = strcat(str, tmp);
    if (!str)
    {
        exit(1);
    }
    return str;
}

void send_message(int socket)
{
    fprintf(stderr, "Enter your message:\n");
    char *line = NULL;
    char *new_line_c = "\n";
    size_t n = 0;
    int len = getline(&line, &n, stdin);
    if (len == -1)
    {
        exit(0);
    }
    if (!strchr(line, '\n'))
    {
        line = concat(line, new_line_c);
        len += 1;
    }
    send(socket, line, len, 0);
    free(line);
}

void recv_message(int socket)
{
    char buff[DEFAULT_BUFFER_SIZE];
    char *str = calloc(1, 20);
    if (!str)
    {
        exit(1);
    }
    int end = 0;
    do
    {
        if ((end = recv(socket, &buff[0], DEFAULT_BUFFER_SIZE - 1, 0)) < 0)
            exit(1);
        buff[end] = '\0';
        str = concat(str, buff);
    } while (!strchr(buff, '\n'));
    printf("Server answered with: %s", str);
    free(str);
}

void communicate(int server_socket)
{
    while (1)
    {
        send_message(server_socket);
        recv_message(server_socket);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        return 1;
    int socket = prepare_socket(argv[1], argv[2]);
    if (socket < 0)
        return 1;
    communicate(socket);
    close(socket);
    return 0;
}