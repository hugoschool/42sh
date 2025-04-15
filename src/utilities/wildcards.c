/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** wildcards.c
*/

#include "mysh.h"

char *revstr(char *str)
{
    int n = strlen(str);
    char temp;

    for (int i = 0; i < n / 2; i++) {
        temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
    return str;
}

int execute_newfile(char *filename, ast_node_t *ast, char *dir_name)
{
    int last_arg = count_args(ast->args) - 1;
    char *n_fname = calloc(strlen(filename) + strlen(dir_name) + 1,
    sizeof(char));

    revstr(filename);
    strcpy(n_fname, dir_name);
    strcat(n_fname, filename);
    free(ast->args[last_arg]);
    ast->args[last_arg] = n_fname;
    return execute_command(ast);
}

char *find_dir(ast_node_t *ast)
{
    char *str = ast->args[count_args(ast->args) - 1];
    char *token = strtok(str, "*");
    char *dir = NULL;

    if (strstr(token, "/") != NULL) {
        dir = strdup(token);
        return dir;
    } else
        return "./";
}

int found_files(ast_node_t *ast, char *end)
{
    char *dir_name = find_dir(ast);
    DIR *dir = opendir(dir_name);
    struct dirent *file = NULL;
    int ret = 0;

    if (dir == NULL)
        return 84;
    file = readdir(dir);
    revstr(end);
    while (file != NULL) {
        revstr(file->d_name);
        if (strncmp(end, file->d_name, strlen(end)) == 0) {
            ret = execute_newfile(file->d_name, ast, dir_name);
        }
        file = readdir(dir);
    }
    closedir(dir);
    return ret;
}

int wildcard(ast_node_t *ast)
{
    int ret = 0;
    char *end = NULL;
    int last = count_args(ast->args) - 1;
    char *token = strtok(ast->args[last], "*");

    token = strtok(NULL, "*");
    if (token != NULL)
        end = strdup(token);
    if (token == NULL && strstr(ast->args[last], "*") != NULL)
        end = strdup(&(strstr(ast->args[last], "*")[1]));
    if (end != NULL) {
        found_files(ast, end);
    } else
        ret = execute_command(ast);
    return ret;
}
