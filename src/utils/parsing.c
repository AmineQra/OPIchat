#include "parsing.h"

#include <regex.h>
#include <stddef.h>
#include <string.h>

#include "tools.h"
#include "xalloc.h"

struct param_tuple *get_params_values(struct param_tuple *par_ref, char *params)
{
    par_ref->param1 = NULL;
    par_ref->param2 = NULL;
    char *cpy_params = xcalloc(strlen(params), sizeof(char) * 2);
    cpy_params = strcpy(cpy_params, params);
    char *temp = strtok(cpy_params, "\n");
    char *left_part = NULL;
    char *right_part = NULL;
    while (temp)
    {
        split(temp, &left_part, &right_part);
        if (left_part && right_part)
        {
            if (par_ref->name_param1
                && strequals(left_part, par_ref->name_param1))
            {
                par_ref->param1 = xfree_null(par_ref->param1);
                par_ref->param1 = xcalloc(strlen(right_part), sizeof(char) * 2);
                strcpy(par_ref->param1, right_part);
            }
            if (par_ref->name_param2
                && strequals(left_part, par_ref->name_param2))
            {
                par_ref->param2 = xfree_null(par_ref->param2);
                par_ref->param2 = xcalloc(strlen(right_part), sizeof(char) * 2);
                strcpy(par_ref->param2, right_part);
            }
        }
        left_part = xfree_null(left_part);
        right_part = xfree_null(right_part);
        temp = strtok(NULL, "\n");
    }
    cpy_params = xfree(cpy_params);
    return par_ref;
}

char *tuple_to_char(struct param_tuple *param_tuple)
{
    char *str = xcalloc(10, sizeof(char));
    str = concat(str, param_tuple->name_param1);
    str = concat(str, "=");
    if (param_tuple->param1)
        str = concat(str, param_tuple->param1);
    else
        str = concat(str, "Bro");
    if (param_tuple->name_param2)
    {
        str = concat(str, "\n");
        str = concat(str, param_tuple->name_param2);
        str = concat(str, "=");
        if (param_tuple->param2)
            str = concat(str, param_tuple->param2);
        else
            str = concat(str, "Bro");
    }
    str = concat(str, "\n");
    return str;
}

int char_matching_pattern(char *str, char *pattern)
{
    regex_t reg;
    int reti = regcomp(&reg, pattern, 0);
    if (reti)
    {
        return 0;
    }
    reti = regexec(&reg, str, 0, NULL, 0);
    if (reti == 0)
    {
        return 1;
    }
    return 0;
}