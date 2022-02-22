#ifndef LIST_H
#define LIST_H

/**
** \brief Generic implementaion of a linked list
**
** \param value Generic pointer to a data to store.
** \param next_node Pointer to the next node, NULL if no node.
*/
struct l_list
{
    void *value;
    struct l_list *next_node;
};

/**
** \brief Init the first node of the linked list
**
** \param value The value of the node.
** \return Return the node wich is the head of the list.
*/
struct l_list *init_list(void *value);

/**
** \brief Add a value at the end of the list if the value is not already in the
** list.
**
** \param list The list to work on.
** \param value The value to add at the end.
** \param comparator A function pointer that allow to know to compare if the
** value is already in the list.
** The first comparator's param is the node's value, the second one correspond
** to the value to add.
** \param err Will be set to 1 if the value is already in the list.
** \return Return the head of the list.
**
** If the list is umpty (NULL), a call to init_list is done.
** If the
*/
struct l_list *add_value(struct l_list *list, void *value,
                         int (*comparator)(void *, void *), int *err);

/**
** \brief Remove the specified value if exist in the list.
**
** \param list The list to work on.
** \param value The value to remove.
** \param comparator A function pointer that allow to know if the current value
** is the value to remove.
** The first comparator's param is the node's value, the second one correspond
** to the value to remove.
** \param err Will be set to 1 if the value wasn't in the list.
** \param ptr_old_val Will point to the value removed.
** \return Return the head of the list.
**
** If the list is umpty (NULL), a call to init_list is done.
*/
struct l_list *remove_value(struct l_list *list, void *value,
                            int (*comparator)(void *, void *), int *err,
                            void **ptr_old_val);

/**
** \brief Iterate on the list to find the value.
**
** \param list The list to work on.
** \param value The value to find.
** \param comparator A function pointer that allow to know if the current value
** is the value to find.
** The first comparator's param is the node's value, the second one correspond
** to the value to find.
** \return Return 1 of the value is found, 0 otherwise.
**
** If the list is umpty (NULL), a call to init_list is done.
*/
int already_in_list(struct l_list *list, void *value,
                    int (*comparator)(void *, void *));

/**
** \brief Return the node node that contain the value according to the
** comparator function.
**
** \param list The list to work on.
** \param value The value to find in a node.
** \param comparator A function pointer that allow to know if the current node's
** value contains the value.
** The first comparator's param is the node's value, the second one correspond
*to the value to find.
** \return Return the node if found, NULL otherwise.
*/
void *get_node(struct l_list *list, void *value,
               int (*comparator)(void *, void *));

/**
** \brief Remove the specified value if exist in the list.
**
** \param list The list to free.
** \param free_fun A function pointer used to free the value of the node.
** \return NULL.
*/
void *free_list(struct l_list *list, void *(*free_fun)(void *));

int len(struct l_list *list);

#endif /* LIST_H */