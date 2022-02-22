#include "notification.h"

#include <string.h>

#include "../package.h"
#include "../tools.h"
#include "../xalloc.h"

void notif_send_dm_brdc(struct package *pck)
{
    char *from = NULL;
    char *temp = strtok(pck->params, "\n");
    char *left_part = NULL;
    char *right_part = NULL;
    while (temp)
    {
        split(temp, &left_part, &right_part);
        if (left_part && right_part)
        {
            if (strequals(left_part, "From"))
            {
                from = xfree_null(from);
                from = xcalloc(strlen(right_part), sizeof(char) * 2);
                strcpy(from, right_part);
            }
        }
        left_part = xfree_null(left_part);
        right_part = xfree_null(right_part);
        temp = strtok(NULL, "\n");
    }
    fprintf(stdout, "From %s: %s\n", from, pck->payload);
}

void notif_send_room(struct package *pck)
{
    char *from = NULL;
    char *room = NULL;
    char *temp = strtok(pck->params, "\n");
    char *left_part = NULL;
    char *right_part = NULL;
    while (temp)
    {
        split(temp, &left_part, &right_part);
        if (left_part && right_part)
        {
            if (strequals(left_part, "From"))
            {
                from = xfree_null(from);
                from = xcalloc(strlen(right_part), sizeof(char) * 2);
                strcpy(from, right_part);
            }
            if (strequals(left_part, "Room"))
            {
                room = xfree_null(room);
                room = xcalloc(strlen(right_part), sizeof(char) * 2);
                strcpy(room, right_part);
            }
        }
        left_part = xfree_null(left_part);
        right_part = xfree_null(right_part);
        temp = strtok(NULL, "\n");
    }
    fprintf(stdout, "From %s@%s: %s\n", from, room, pck->payload);
}