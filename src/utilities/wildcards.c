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

bool ret_and_free(char *str, bool bl)
{
    free(str);
    return bl;
}

static void update_token(char *token, char *last_slash)
{
    int ver = 0;

    for (size_t i = 0; i < strlen(token); i++) {
        if (ver == 1)
            token[i] = '\0';
        if (last_slash == &token[i])
            ver = 1;
    }
}

static bool is_dir_empty(char const *d_name)
{
    DIR *dir = opendir(d_name);
    struct dirent *file = NULL;
    int i = 0;

    printf("%s\n", d_name);
    if (dir != NULL) {
        file = readdir(dir);
        for (; file != NULL; i++)
            file = readdir(dir);
        closedir(dir);
        if (i <= 2)
            return true;
        return false;
    }
    return true;
}

bool my_acces_star(char *str)
{
    char *token = strtok(str, "*");
    char *last_slash = NULL;

    if (strlen(str) == 1)
        return ret_and_free(str, true);
    if (strlen(token) <= 1)
        return ret_and_free(str, false);
    last_slash = strrchr(token, '/');
    if (last_slash != NULL) {
        update_token(token, last_slash);
        if (is_dir_empty(token))
            return ret_and_free(str, false);
        return ret_and_free(str, true);
    }
    return ret_and_free(str, true);
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
    if (strchr(str, '[') != NULL)
        return true;
    return false;
}

int wildcard(ast_node_t *ast)
{
    int last = count_args(ast->args) - 1;
    glob_t *globbuff = calloc(1, sizeof(glob_t));

    if (last > 0 && is_card(ast->args[last])) {
        if (!my_acces_star(strdup(ast->args[last])) &&
        strstr(ast->args[last], "*") != NULL)
            execute_command_path(ast->args, -1);
        globbuff->gl_offs = last;
        glob(ast->args[last], GLOB_DOOFFS, NULL, globbuff);
        for (int i = 0; i < last; i++) {
            globbuff->gl_pathv[i] = strdup(ast->args[i]);
        }
        free_args(ast->args);
        ast->args = my_dup_array(globbuff->gl_pathv);
    }
    globfree(globbuff);
    execute_command_path(ast->args, 0);
    return 0;
}
