/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_unsetenv.c
*/

#include "mysh.h"

/**
 * @brief Checks the arguments for unsetenv command.
 *
 * @param args : Arguments passed to the unsetenv command.
 * @return : 1 if error is found, 0 otherwise.
 */
int check_unsetenv(char *args[])
{
    if (!args[1])
        return print_error(get_error_msg(ERR_UNSETENV_TOO_FEW), NULL, 1);
    if (strcmp(args[1], "*") == 0)
        return print_error(get_error_msg(ERR_UNSETENV_ALPHA_CAR), NULL, 1);
    return 0;
}

/**
 * @brief Copies a variable from the old environment to the new environment.
 *
 * @param new_environ : The new environment array.
 * @param data : The structure containing environment data.
 * @param j : The index for the new environment.
 * @param i : The index for the old environment.
 * @return : 1 on success, 0 if memory allocation fails.
 */
static int copy_variable(char **new_environ, unset_data_t *data, int *j, int i)
{
    new_environ[*j] = strdup(data->old_env[i]);
    if (!new_environ[*j]) {
        for (int k = 0; k < *j; k++)
            free(new_environ[k]);
        free(new_environ);
        return 0;
    }
    (*j)++;
    return 1;
}

/**
 * @brief Removes a specified variable from the environment.
 *
 * @param data : The structure containing environment data.
 * @return : A new environment array without the removed variable, or NULL
 * on failure.
 */
static char **remove_variable(unset_data_t *data)
{
    int j = 0;
    char **new_environ = malloc(sizeof(char *) * (data->env_size + 1));

    if (!new_environ)
        return NULL;
    for (int i = 0; data->old_env[i]; i++) {
        if (strncmp(data->old_env[i], data->var, data->var_len) == 0 &&
        data->old_env[i][data->var_len] == '=') {
            data->found = 1;
            continue;
        }
        if (!copy_variable(new_environ, data, &j, i))
            return NULL;
    }
    new_environ[j] = NULL;
    return new_environ;
}

/**
 * @brief Removes a variable from the environment.
 *
 * @param args : The array of arguments, where args[1] is the variable to
 * remove.
 * @return : 0 on success, 1 on error with message.
 */
int my_unsetenv(char *args[])
{
    extern char **environ;
    unset_data_t data = {environ, NULL, args[1], 0, 0, 0};
    char **new_environ;

    if (!args[1] || (args[1] && args[2]))
        return print_error((!args[1]) ? get_error_msg(ERR_UNSETENV_TOO_FEW)
        : get_error_msg(ERR_UNSETENV_WRONG_NUM), NULL, 1);
    if (strcmp(args[1], "*") == 0)
        return print_error(get_error_msg(ERR_UNSETENV_WRONG_ARG), NULL, 1);
    while (data.old_env[data.env_size])
        data.env_size++;
    data.var_len = strlen(data.var);
    new_environ = remove_variable(&data);
    if (!new_environ)
        return 0;
    environ = new_environ;
    return 0;
}
