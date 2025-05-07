/*
** EPITECH PROJECT, 2025
** lib_list
** File description:
** File for handling all about linked list
*/

#include "list.h"
#include <stdlib.h>

int len_list(llist_t *list)
{
    int len = 1;

    while (list != NULL) {
        len++;
        list = list->next;
    }
    return len;
}

void free_list(llist_t **list)
{
    llist_t *temp = *list;

    if (*list == NULL)
        return;
    while (*list != NULL) {
        *list = (*list)->next;
        free(temp);
        temp = *list;
    }
}

void free_list_obj(llist_t **list)
{
    llist_t *temp = *list;

    while ((*list)->next != NULL) {
        (*list) = (*list)->next;
        if (temp->data != NULL)
            free(temp->data);
        free(temp);
        temp = *list;
    }
    free(temp);
}

void cat_list(llist_t **dest, llist_t *node, int where)
{
    llist_t *temp = *dest;

    if (where == START) {
        node->next = *dest;
        *dest = node;
    } else {
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
    }
}

llist_t *create_node(void *entity)
{
    llist_t *node = malloc(sizeof(llist_t));

    if (node == NULL)
        return NULL;
    node->data = entity;
    node->next = NULL;
    return node;
}
