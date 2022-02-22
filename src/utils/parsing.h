#ifndef PARSING_H
#define PARSING_H

/**
** \brief the contains two token names and two placed holder to store
** there value.
**
** \param name_param1 A char * containing the name of the first token.
** \param name_param2 A char * containing the name of the second token.
** Can be NULL if there is no second token
** \param param1 Contain the value of the first token if found else NULL.
** \param param1 Contain the value of the second token if found else NULL.
*/
struct param_tuple
{
    char *name_param1;
    char *name_param2;
    char *param1;
    char *param2;
};

/**
** \brief Search param token, contains in the struct, in the char * params. If
** they exits there values are stored into the structure.
**
** \param par_ref The struct pointer that contains the tokens to find.
** \param params The char * containing all the receved params.
** \return Return the struct containing the token's values if they were found.
*/
struct param_tuple *get_params_values(struct param_tuple *par_ref,
                                      char *params);

char *tuple_to_char(struct param_tuple *param_tuple);

/**
** \brief Match a char pointer to a regex pattern.
**
** \param str The char * to match.
** \param pattern The char * containing the regex pattern.
** \return Return 1 match or 0 if no match or the parttern is not a valid regex
*pattern.
*/
int char_matching_pattern(char *str, char *pattern);

#endif /* PARSING_H */