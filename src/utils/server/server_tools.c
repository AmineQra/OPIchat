#include "server_tools.h"

#include <stdio.h>
#include <string.h>

#include "../package.h"
#include "../tools.h"
#include "../xalloc.h"
#include "connection.h"

int bad_request(int client_socket)
{
    fprintf(stdout, "< Request in:\n0\n0\nINVALID\n\n\n");
    fprintf(stdout, "< Request out:\n12\n3\nINVALID\n\nBad request\n\n");
    send_message("12\n3\nINVALID\n\nBad request\n", client_socket);
    return 1;
}

int request_out(int sock, char *mid_msg, char *params, char *payload)
{
    char *cpy_payload = xcalloc(strlen(payload), sizeof(char) * 2);
    strcpy(cpy_payload, payload);
    char *str =
        char_for_package(strlen(cpy_payload), mid_msg, params, cpy_payload);
    fprintf(stdout, "< Request out:\n%s\n", str);
    int sended = send_message(str, sock);
    str = xfree(str);
    return sended;
}

char *dup_params(char *to_dup)
{
    char *params = xcalloc(strlen(to_dup), sizeof(char) * 2);
    params = strcpy(params, to_dup);
    if (params[0] != '\0')
        params = concat(params, "\n");
    return params;
}

int compare_sock(void *ptr_i1, void *ptr_i2)
{
    int *i1 = ptr_i1;
    int *i2 = ptr_i2;
    return *i1 == *i2;
}