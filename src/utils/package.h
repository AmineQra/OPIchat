#ifndef PACKAGE_H
#define PACKAGE_H

#include <stddef.h>

/**
** \brief Struct that store differents intels related to the package.
**
** \param payload_size The size of the payload.
** \param status The status (0 request, 1 response, 2 notificaton, 3 error).
** \param cmd_name The name of the command.
** \param params A char * containing x time the pattern:
**  charaters=charaters\\n, + \\n at the end.
** \param payload A char * containing char or can be umpty but at least null
** terminate \\0
*/
struct package
{
    size_t payload_size;
    size_t status;
    char *cmd_name;
    char *params;
    char *payload;
};

/**
** \brief Init a struct package and return a pointer. the user must free the
** pointer later
**
** \param payload_size The size of the payload.
** \param status The status of the request.
** \param cmd_name The command name.
** \param params The char * correponding to all the parameters.
** \param payload The char * corresponding to the payload.
** \return Return the struct.
*/
struct package *build_package(size_t payload_size, size_t status,
                              char *cmd_name, char *params, char *payload);

/**
** \brief Change the payload and it's size in the package. The old payload is
** free.
**
** \param package The package to update.
** \param payload_size The size of the payload.
** \param payload The new payload.
** \return Return the concat versoin on all parameters in a char *.
*/
void update_package_payload(struct package *pck, size_t payload_size,
                            char *payload);

/**
** \brief Transform a struct package into a char * ready to send.
**
** \param package The package to transform
** \return Return the concat versoin on all parameters in a char *.
*/
char *package_to_char(struct package *pck);

/**
** \brief Cast if needed and concat the parameter and put then in a single
** char.
**
** \param p_size The size (len) of the param payload
** \param middle_part The char * not user allocated corresponding to
** \\nStatus\\nCmd_name\\n
** \param params The char * with all the params.
** \param payload The char * payload.
** \return Return the message to send.
**
** params and payload are free in the function and params can be NULL
*/
char *char_for_package(size_t p_size, char *middle_part, char *params,
                       char *payload);

/**
** \brief Transform a char * into a struct package.
**
** \param str The char * to transform
** \return Return package version of the string.
*/
struct package *char_to_package(char *raw_str, int *err);

/**
** \brief Free a package and all is contains elements.
**
** \param pck The package to free
** \return NULL.
*/
void *free_package(struct package *pck);

#endif /* PACKAGE_H */