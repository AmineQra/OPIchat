#ifndef COMMANDS_H
#define COMMANDS_H

/**
** \brief Struct that store differents intels related to the command.
**
** \param name The pointer of the command name.
** \param nb_param The number of token.
** \param param1 The name of the first param token, NULL if no token needed.
** \param param2 The name of the second param token, NULL if no token needed.
** \param stay_at_payload Set to 1 if the client as to stay at the payload
** phase, 0 otherwise.
*/
struct command
{
    /* data */
    char *name;
    int nb_param;
    char *param1;
    char *param2;
    int stay_at_payload;
};

/**
** \brief Give the struct command corespondinf to the name.
**
** \param command_name The pointer of the command name.
** \return The pointer to the struct command corresponding to the name, null if
** the name doesn't exit.
*/
struct command *get_struct_command(char *command_name);

/**
** \brief Free the command struct.
**
** \param cmd The pointer of the command.
** \return NULL.
*/
void *free_command(struct command *cmd);

#endif /* COMMANDS_H */