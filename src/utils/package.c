#include "package.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"
#include "xalloc.h"

struct package *build_package(size_t payload_size, size_t status,
                              char *cmd_name, char *params, char *payload)
{
    struct package *pck = xcalloc(1, sizeof(struct package));
    char *name = xcalloc(strlen(cmd_name), sizeof(char) * 2);
    strcpy(name, cmd_name);
    pck->payload_size = payload_size;
    pck->status = status;
    pck->cmd_name = name;
    pck->params = params;
    pck->payload = payload;
    return pck;
}

void update_package_payload(struct package *pck, size_t payload_size,
                            char *payload)
{
    pck->payload_size = payload_size;
    pck->payload = xfree(pck->payload);
    pck->payload = payload;
}

char *package_to_char(struct package *pck)
{
    char *str = xcalloc(1, 2048);
    sprintf(str, "%zu", pck->payload_size);

    char *str_stat = xcalloc(1, 2048);
    sprintf(str_stat, "%zu", pck->status);

    str = concat(str, "\n");
    str = concat(concat(str, str_stat), "\n");
    str = concat(concat(str, pck->cmd_name), "\n");
    str = concat(str, pck->params);
    str = concat(str, pck->payload);
    str_stat = xfree(str_stat);
    return str;
}

char *char_for_package(size_t p_size, char *middle_part, char *params,
                       char *payload)
{
    char *str = xcalloc(5, sizeof(char));

    // Payload size
    char *s_p_size = xcalloc(1, 2048);
    sprintf(s_p_size, "%zu", p_size);
    str = concat(str, s_p_size);
    s_p_size = xfree(s_p_size);

    // Status and command name
    str = concat(str, middle_part);

    // Params
    if (params)
    {
        str = concat(str, params);
        params = xfree(params);
    }
    str = concat(str, "\n");

    // Payload
    str = concat(str, payload);
    payload = xfree(payload);
    return str;
}

struct package *char_to_package(char *raw_str, int *err)
{
    const char *sep = "\n";

    // ******** SPLIT PART ********
    char *str = multi_tok(&raw_str, "\n\n");
    // str doesn't have to be free in this function
    // raw_str contains the payload

    // Split other attributs
    char *s_payload_size = strtok(str, sep);
    char *s_status = strtok(NULL, sep);
    char *cmd_name = strtok(NULL, sep);
    if (!s_payload_size || !s_status || !cmd_name)
    {
        *err = 1;
        return NULL;
    }

    // ******** FORMAT PART ********

    // Payload_size management
    char *endptr = NULL;
    int payload_size = strtol(s_payload_size, &endptr, 10);
    if (s_payload_size == endptr)
    {
        *err = 1;
        return NULL;
    }

    // Cmd_name management
    // nithing to do

    // Status management
    endptr = NULL;
    int status = strtol(s_status, &endptr, 10);
    if (s_status == endptr)
    {
        *err = 1;
        return NULL;
    }

    // Params part
    char *s_params = strtok(NULL, "\0");
    if (!s_params)
        s_params = "";
    char *params = xcalloc(strlen(s_params), sizeof(char *));
    params = strcpy(params, s_params);

    // Payload management
    if (raw_str == NULL)
        raw_str = "";
    char *payload = xcalloc(strlen(raw_str), sizeof(char *));
    payload = strcpy(payload, raw_str);

    return build_package(payload_size, status, cmd_name, params, payload);
}

void *free_package(struct package *pck)
{
    pck->cmd_name = xfree(pck->cmd_name);
    pck->params = xfree(pck->params);
    pck->payload = xfree(pck->payload);
    pck = xfree(pck);
    return NULL;
}