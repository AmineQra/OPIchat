#ifndef TOOLS_H
#define TOOLS_H

#include <stddef.h>
#include <stdio.h>

enum status
{
    REQUEST = 0,
    RESPONSE = 1,
    NOTIFICATION = 2,
    ERROR = 3
};

/**
** \brief Strcat wrapper that exit on failure.
**
** \param str The mem pointer of the first string.
** \param tmp The mem pointer of the second string.
** \return The concat version of the two param in str.
*/
char *concat(char *str, char *tmp);

/**
** \brief Strcat wrapper that exit on failure.
**
** \param str The mem pointer of the first string.
** \param tmp The mem pointer of the second string.
** \return The concat version of the two param in str.
*/
char *concatn(char *str, char *tmp, size_t n);

/**
** \brief Strcmp wrapper that exit on failure.
**
** \param str The mem pointer of the first string.
** \param tmp The mem pointer of the second string.
** \return 1 if they are equals, 0 otherwise.
*/
int strequals(char *str, char *tmp);

/**
** \brief Strcmp wrapper that exit on failure.
**
** \param ptr1 The mem pointer of the first string.
** \param ptr2 The mem pointer of the second string.
** \return 1 if they are equals, 0 otherwise.
*/
int xstrequals(void *ptr1, void *ptr2);

/**
** \brief Split the string in to parts corresponding of the left and right side
*of the char '='.
**
** \param str_to_split The mem pointer of the string to split.
** \param left_part The mem pointer of pointer of the left string.
** \param right_part The mem pointer of pointer of the right string.
** \return nothing.
*/
void split(char *str_to_split, char **left_part, char **right_part);

/**
** \brief Beautiful function
**
** \param input The mem pointer of the string to split.
** \param delimeter The mem pointer of pointer of the left string.
** \return nothing.
*/
char *multi_tok(char **input, char *delimiter);

/**
** \brief getline wrapper that remove the \ n at the end of the char*.
**
** \param lineptr The mem pointer to the string.
** \param n The pointer that will store the bits read.
** \param stream The file descriptor to read.
** \return nothing.
*/
int xgetline(char **lineptr, size_t *n, FILE *stream);

#endif /* TOOLS_H */