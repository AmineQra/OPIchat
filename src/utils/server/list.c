#include "list.h"

#include <stdio.h>

#include "../xalloc.h"

struct l_list *init_list(void *value)
{
    struct l_list *list = xcalloc(1, sizeof(struct l_list));
    list->value = value;
    list->next_node = NULL;
    return list;
}

struct l_list *add_value(struct l_list *list, void *value,
                         int (*comparator)(void *, void *), int *err)
{
    if (list == NULL)
        return init_list(value);
    struct l_list *ptr = list;
    struct l_list *ptr_prec = list;
    while (ptr)
    {
        if (comparator(ptr->value, value))
        {
            *err = 1;
            return list;
        }
        ptr_prec = ptr;
        ptr = ptr->next_node;
    }
    struct l_list *node = init_list(value);
    ptr_prec->next_node = node;
    return list;
}

struct l_list *remove_value(struct l_list *list, void *value,
                            int (*comparator)(void *, void *), int *err,
                            void **ptr_old_val)
{
    *err = 1;
    if (!list)
        return NULL;
    if (comparator(list->value, value))
    {
        *err = 0;
        *ptr_old_val = list->value;
        struct l_list *next = list->next_node;
        xfree(list);
        return next;
    }

    struct l_list *ptr = list->next_node;
    struct l_list *prev_ptr = list;
    do
    {
        if (comparator(ptr->value, value))
        {
            *err = 0;
            *ptr_old_val = ptr->value;
            prev_ptr->next_node = ptr->next_node;
            xfree(ptr);
            return list;
        }
        prev_ptr = ptr;
        ptr = ptr->next_node;
    } while (ptr);
    return list;
}

int already_in_list(struct l_list *list, void *value,
                    int (*comparator)(void *, void *))
{
    struct l_list *ptr = list;
    while (ptr)
    {
        if (comparator(ptr->value, value))
            return 1;
        ptr = ptr->next_node;
    }
    return 0;
}

void *get_node(struct l_list *list, void *value,
               int (*comparator)(void *, void *))
{
    struct l_list *ptr = list;
    while (ptr)
    {
        if (comparator(ptr->value, value))
        {
            return ptr->value;
        }
        ptr = ptr->next_node;
    }
    return NULL;
}

void *free_list(struct l_list *list, void *(*free_fun)(void *))
{
    free_fun(list->value);
    if (list->next_node)
        free_list(list->next_node, free_fun);
    list = xfree(list);
    return NULL;
}

int len(struct l_list *list)
{
    struct l_list *ptr = list;
    int size = 0;
    while (ptr)
    {
        size += 1;
        ptr = ptr->next_node;
    }
    return size;
}