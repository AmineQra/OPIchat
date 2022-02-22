#include "tools.h"

#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "xalloc.h"

char *concat(char *str, char *tmp)
{
    str = xrealloc(str, strlen(str) + strlen(tmp) + 1);

    str = strcat(str, tmp);
    if (!str)
        errx(1, "[TOOLS] [CONCAT] strcat failed");

    return str;
}

char *concatn(char *str, char *tmp, size_t n)
{
    str = xrealloc(str, strlen(str) + n + 1);

    str = strncat(str, tmp, n);
    if (!str)
        errx(1, "[TOOLS] [CONCATN] strncat failed");

    return str;
}

int strequals(char *str, char *tmp)
{
    int res = strcmp(str, tmp);
    if (res == 0)
        return 1;
    return 0;
}

int xstrequals(void *ptr1, void *ptr2)
{
    char *s1 = ptr1;
    char *s2 = ptr2;
    return strequals(s1, s2);
}

void split(char *str_to_split, char **left_part, char **right_part)
{
    if (!str_to_split)
        errx(1, "[TOOLS] [SPLIT] umpty string to split");

    *left_part = xcalloc(16, sizeof(char) * 2);
    *right_part = xcalloc(16, sizeof(char) * 2);

    int found = 0;

    for (int i = 0; str_to_split[i] != '\0'; i++)
    {
        if (found)
            *right_part = concatn(*right_part, &str_to_split[i], 1);

        else if (str_to_split[i] == '=')
        {
            found = 1;
            *left_part = concat(*left_part, "\0");
        }
        else
            *left_part = concatn(*left_part, &str_to_split[i], 1);
    }
}

char *multi_tok(char **input, char *delimiter)
{
    if (*input == NULL)
        return NULL;

    char *end = strstr(*input, delimiter);
    if (end == NULL)
    {
        char *temp = *input;
        *input = NULL;
        return temp;
    }

    char *temp = *input;

    *end = '\0';
    *input = end + strlen(delimiter); // move pointer
    return temp;
}

int xgetline(char **lineptr, size_t *n, FILE *stream)
{
    int res = getline(lineptr, n, stream);
    if (res > 0)
    {
        char *line = *lineptr;
        line[strlen(line) - 1] = '\0';
        if (line[0] == '\0')
            line[0] = '\n';
    }
    return res;
}