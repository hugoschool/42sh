/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** wildcards.c
*/

#include "mysh.h"

char **my_dup_array(char **array)
{
    char **n_array = NULL;

    if (array == NULL)
        return NULL;
    n_array = calloc(count_args(array) + 1, sizeof(char *));
    for (int i = 0; i < count_args(array); i++)
        n_array[i] = strdup(array[i]);
    return n_array;
}

bool is_card(char *str)
{
    if (strchr(str, '*') != NULL)
        return true;
    if (strchr(str, '?') != NULL)
        return true;
    if (strchr(str, '+') != NULL)
        return true;
    if (strchr(str, '@') != NULL)
        return true;
    if (strchr(str, '!') != NULL)
        return true;
    return false;
}

int wildcard(ast_node_t *ast)
{
    int ret = 0;
    int last = count_args(ast->args) - 1;
    glob_t *globbuff = calloc(1, sizeof(glob_t));

    if (last > 0 && is_card(ast->args[last])) {
        globbuff->gl_offs = last;
        glob(ast->args[last], GLOB_DOOFFS, NULL, globbuff);
        for (int i = 0; i < last; i++) {
            globbuff->gl_pathv[i] = strdup(ast->args[i]);
        }
        free_args(ast->args);
        ast->args = my_dup_array(globbuff->gl_pathv);
    }
    globfree(globbuff);
    execute_command(ast);
    return ret;
}
char *end = NULL;
