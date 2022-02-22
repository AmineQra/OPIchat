#include "commands.h"

#include <string.h>

#include "tools.h"
#include "xalloc.h"

static struct command *set_cmd_val(struct command *cmd, char *name,
                                   int nb_param, char *param1, char *param2,
                                   int stay_at_payload)
{
    char *cmd_name = xcalloc(strlen(name), sizeof(char) * 2);
    strcpy(cmd_name, name);
    cmd->name = cmd_name;
    cmd->nb_param = nb_param;
    cmd->param1 = param1;
    cmd->param2 = param2;
    cmd->stay_at_payload = stay_at_payload;
    return cmd;
}

struct command *get_struct_command(char *command_name)
{
    struct command *cmd = xcalloc(1, sizeof(struct command));

    if (strequals(command_name, "PING"))
        return set_cmd_val(cmd, "PING", 0, NULL, NULL, 0);
    if (strequals(command_name, "LOGIN"))
        return set_cmd_val(cmd, "LOGIN", 0, NULL, NULL, 0);
    if (strequals(command_name, "LIST-USERS"))
        return set_cmd_val(cmd, "LIST-USERS", 0, NULL, NULL, 0);
    if (strequals(command_name, "SEND-DM"))
        return set_cmd_val(cmd, "SEND-DM", 2, "User", "From", 1);
    if (strequals(command_name, "BROADCAST"))
        return set_cmd_val(cmd, "BROADCAST", 1, "From", NULL, 1);
    if (strequals(command_name, "CREATE-ROOM"))
        return set_cmd_val(cmd, "CREATE-ROOM", 0, NULL, NULL, 0);
    if (strequals(command_name, "LIST-ROOMS"))
        return set_cmd_val(cmd, "LIST-ROOMS", 0, NULL, NULL, 0);
    if (strequals(command_name, "JOIN-ROOM"))
        return set_cmd_val(cmd, "JOIN-ROOM", 0, NULL, NULL, 0);
    if (strequals(command_name, "LEAVE-ROOM"))
        return set_cmd_val(cmd, "LEAVE-ROOM", 0, NULL, NULL, 0);
    if (strequals(command_name, "SEND-ROOM"))
        return set_cmd_val(cmd, "SEND-ROOM", 2, "Room", "From", 1);
    if (strequals(command_name, "DELETE-ROOM"))
        return set_cmd_val(cmd, "DELETE-ROOM", 0, NULL, NULL, 0);
    if (strequals(command_name, "PROFILE"))
        return set_cmd_val(cmd, "PROFILE", 0, NULL, NULL, 0);
    return NULL;
}

void *free_command(struct command *cmd)
{
    cmd->name = xfree(cmd->name);
    // cmd->param1 no free because declare with ""
    // cmd->param2 no free because declare with ""
    cmd = xfree(cmd);
    return NULL;
}
