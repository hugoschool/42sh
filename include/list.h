/*
** EPITECH PROJECT, 2024
** my_radar
** File description:
** Macro for a linked list
*/

#ifndef LIST_H_
    #define LIST_H_

    #define START 1
    #define END 0

typedef struct llist {
    void *data;
    struct llist *next;
} llist_t;

void free_list(llist_t **list);
void free_list_obj(llist_t **list);
void cat_list(llist_t **dest, llist_t *node, int where);
llist_t *create_node(void *entity);
int len_list(llist_t *list);
char **list_to_arr(llist_t **list);
llist_t *arr_to_list(char **arr);

#endif /* LIST_H_ */
