#include "client.h"

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils/client/notification.h"
#include "utils/client/thread.h"
#include "utils/commands.h"
#include "utils/frees.h"
#include "utils/package.h"
#include "utils/tools.h"
#include "utils/xalloc.h"

int create_and_connect(struct addrinfo *addrinfo)
{
    if (addrinfo == NULL)
    {
        errx(1, "[CLIENT] [CREATE&CONNECT] addrinfo is null");
    }
    for (struct addrinfo *curs = addrinfo; curs != NULL; curs = curs->ai_next)
    {
        int s = socket(curs->ai_family, curs->ai_socktype, curs->ai_protocol);
        if (s >= 0 && !connect(s, curs->ai_addr, curs->ai_addrlen))
        {
            return s;
        }
    }

    errx(1, "[CLIENT] [CREATE&CONNECT] failed to connect");
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
        errx(1, "[CLIENT] [PREPARE_SOCKET] failed getaddrinfo");

    int socker = create_and_connect(info_addr_serv);
    if (socker)
    {
        freeaddrinfo(info_addr_serv);
        return socker;
    }
    errx(1, "[CLIENT] [PREPARE_SOCKET] failed prepare socket");
}

static struct command *get_command(int *status)
{
    struct command *cmd = NULL;
    while (1)
    {
        fprintf(stdout, "Command:\n");
        char *cmd_name = NULL;
        size_t n = 0;
        if (xgetline(&cmd_name, &n, stdin) < 0)
        {
            // it's a normal way to exit the program so err not needed
            cmd_name = xfree(cmd_name);
            *status = SHUTDOWN;
            return NULL;
        }

        cmd = get_struct_command(cmd_name);
        cmd_name = xfree(cmd_name);
        if (cmd)
            return cmd;
        fprintf(stderr, "Invalid command\n");
    }
}

static char *get_param(char *params, int *status)
{
    fprintf(stdout, "Parameters:\n");

    char *line = NULL;
    size_t n = 0;
    ssize_t len = xgetline(&line, &n, stdin);

    char *left_part = NULL;
    char *right_part = NULL;
    while (strcmp(line, "\n"))
    {
        if (len < 0)
        {
            // it's a normal way to exit the program so err not needed
            line = xfree(line);
            *status = SHUTDOWN; // used to manage thread
            return params;
        }

        char *tmp = xcalloc(strlen(line), sizeof(char) * 2);
        tmp = strcpy(tmp, line);
        left_part = strtok(tmp, "=");
        right_part = strtok(NULL, "=");
        free(tmp);
        if (right_part && left_part)
        {
            params = concat(concat(params, line), "\n");
        }
        else
            fprintf(stderr, "Invalid parameter\n");

        // reset
        line = xfree(line);
        n = 0;
        len = xgetline(&line, &n, stdin);
    }
    line = xfree(line);
    return concat(params, "\n"); // concat used to match protocol
}

static int get_payload(int socket, struct command *cmd, char *params, int *err)
{
    struct package *pck = build_package(0, REQUEST, cmd->name, params, NULL);
    do
    {
        fprintf(stdout, "Payload:\n");
        char *line = NULL;
        size_t n = 0;
        if (xgetline(&line, &n, stdin) < 0)
        {
            // it's a normal way to exit the program so err not needed
            free_get_payload(cmd, pck, line);
            return SHUTDOWN; // to shutdown the other thread
        }

        if (strequals(line, "/quit"))
        {
            free_get_payload(cmd, pck, line);
            return RUN;
        }
        if (line[0] == '\n')
            line[0] = '\0';
        update_package_payload(pck, strlen(line), line);
        line = package_to_char(pck);

        int sended = send(socket, line, strlen(line), 0);
        line = xfree(line);
        if (sended == -1)
        {
            free_get_payload(cmd, pck, line);
            *err = 1;
            return SHUTDOWN; // to shutdown the other thread
        }
    } while (cmd->stay_at_payload);
    free_get_payload(cmd, pck, NULL);
    return RUN;
}

static int handel_user_input(int socket, int *err, char **temp)
{
    int status = RUN;
    *temp = NULL;
    struct command *cmd = get_command(&status);
    if (status == SHUTDOWN)
    {
        *err = 1;
        return SHUTDOWN; // shutdown thread
    }
    char *params = xcalloc(1, 16);
    if (cmd->nb_param && !strequals(cmd->name, "BROADCAST"))
        params = get_param(params, &status);
    if (status == SHUTDOWN)
    {
        cmd = free_command(cmd);
        params = xfree(params);
        *err = 1;
        return SHUTDOWN; // shutdown thread
    }
    if (params[0] == '\0')
        params[0] = '\n';
    return get_payload(socket, cmd, params, err);
}

static int parse_response(char *response, int *err)
{
    char *ptr = response;
    struct package *pck = char_to_package(response, err);
    free(ptr);
    if (*err == 1)
    {
        return SHUTDOWN;
    }

    // if it's a normal response
    if (pck->status == 1)
    {
        if (pck->payload_size > 0)
        {
            fprintf(stdout, "< %s", pck->payload);
            // \n already include in the payload
        }
    }
    // if it's a notification
    else if (pck->status == 2)
    {
        if (strequals(pck->cmd_name, "SEND-DM"))
        {
            notif_send_dm_brdc(pck);
        }
        else if (strequals(pck->cmd_name, "BROADCAST"))
        {
            notif_send_dm_brdc(pck);
        }
        else
        {
            notif_send_room(pck);
        }
    }
    // if it's a error return by the server
    else if (pck->status == 3)
    {
        if (pck->payload_size > 0)
        {
            fprintf(stderr, "! %s", pck->payload);
            // \n already include in the payload
        }
    }
    // not a convential case, must exit
    else
    {
        *err = 1;
        pck = free_package(pck);
        return SHUTDOWN;
    }
    pck = free_package(pck);
    return RUN;
}

static int handel_server_input(int socket, int *err, char **temp)
{
    char buff[DEFAULT_BUFFER_SIZE];
    *temp = xcalloc(20, sizeof(char) * 2);
    if (!*temp)
    {
        exit(1);
    }
    int end = 0;
    do
    {
        if ((end = recv(socket, &buff[0], DEFAULT_BUFFER_SIZE - 1, 0)) < 0)
            exit(1);
        buff[end] = '\0';
        *temp = concat(*temp, buff);
    } while (!strchr(buff, '\n'));
    return parse_response(*temp, err);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        errx(1, "usage: /student <ip> <port>");

    int socket = prepare_socket(argv[1], argv[2]);
    if (socket < 0)
        errx(1, "[CLIENT] [MAIN] failed, socket not found (404)");

    int stat_send = RUN;
    int stat_recv = RUN;
    int err = 0;
    char **str = xcalloc(1, sizeof(char *));
    char **___ = xcalloc(1, sizeof(char *));

    pthread_t t_send;
    struct thread_flag *f_send;
    f_send =
        build_thread_flag(socket, &stat_send, &err, ___, handel_user_input);

    pthread_t t_recv;
    struct thread_flag *f_recv;
    f_recv =
        build_thread_flag(socket, &stat_recv, &err, str, handel_server_input);

    pthread_create(&t_send, NULL, *thread_routine, f_send);
    pthread_create(&t_recv, NULL, *thread_routine, f_recv);

    while (stat_recv && stat_send)
        continue;

    if (!stat_send)
        pthread_cancel(t_recv);
    if (!stat_recv)
        pthread_cancel(t_send);

    free(f_send);
    free(f_recv);
    free(*str);
    free(str);
    free(___);
    close(socket);
    return 0;
}