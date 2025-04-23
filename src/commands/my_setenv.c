/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_setenv.c
*/

#include "mysh.h"

/**
 * @brief Checks if a string is a valid identifier.
 *
 * @param str : The string to be validated.
 * @return : 1 if valid, 0 if invalid with error message.
 */
static int is_valid_identifier(const char *str)
{
    if (!str || !(*str))
        return 0;
    if (!((*str >= 'A' && *str <= 'Z') || (*str >= 'a' && *str <= 'z') ||
    *str == '_'))
        return print_error(get_error_msg(ERR_SETENV_BEGIN_LETTER), NULL,
        0);
    for (int i = 1; str[i]; i++) {
        if (!(str[i] == '_' || (str[i] >= 'A' && str[i] <= 'Z') ||
        (str[i] >= 'a' && str[i] <= 'z') ||
        (str[i] >= '0' && str[i] <= '9')))
            return print_error(get_error_msg(ERR_SETENV_ALPHA_CARA), NULL,
            0);
    }
    return 1;
}

/**
 * @brief Allocates memory for a new variable and its value, and copies them.
 *
 * @param var : The variable name.
 * @param var_len : The length of the variable name.
 * @param val : The value of the variable.
 * @param val_len : The length of the value.
 * @return : A pointer to the newly allocated string with
 * the variable and value.
 */
static char *allocate_and_copy_variable(const char *var, size_t var_len,
    const char *val, size_t val_len)
{
    char *new_var = malloc(var_len + val_len + 3);

    if (!new_var)
        return NULL;
    strncpy(new_var, (char *)var, var_len);
    new_var[var_len] = '=';
    if (val)
        strncpy(new_var + var_len + 1, (char *)val, val_len + 1);
    else
        new_var[var_len + 1] = '\0';
    return new_var;
}

/**
 * @brief Copies the old environment and updates it with the new variable.
 *
 * @param data : The structure containing environment data.
 * @return : A pointer to the new environment.
 */
static char **copy_old_env(env_data_t *data)
{
    for (int i = 0; i < data->env_size; i++) {
        if (i == data->var_exists)
            data->new_env[i] = allocate_and_copy_variable(data->var,
            data->var_len, data->val, data->val_len);
        if (i == data->var_exists && !data->new_env[i])
            return NULL;
        if (i != data->var_exists)
            data->new_env[i] = data->old_env[i];
    }
    return data->new_env;
}

/**
 * @brief Adds a new variable to the environment.
 *
 * @param data : The structure containing environment data.
 * @return : A pointer to the updated environment.
 */
static char **add_new_variable(env_data_t *data)
{
    data->new_env[data->env_size] = malloc(data->var_len + data->val_len + 3);
    if (!data->new_env[data->env_size])
        return NULL;
    strncpy(data->new_env[data->env_size], (char *)data->var,
    data->var_len);
    data->new_env[data->env_size][data->var_len] = '=';
    if (data->val)
        strncpy(data->new_env[data->env_size] + data->var_len + 1,
        (char *)data->val, data->val_len + 1);
    else
        data->new_env[data->env_size][data->var_len + 1] = '\0';
    data->new_env[data->env_size + 1] = NULL;
    return data->new_env;
}

/**
 * @brief Finds the index of an existing variable in the environment.
 *
 * @param old_env : The old environment.
 * @param var : The variable to search for.
 * @param var_len : The length of the variable name.
 * @return : The index of the variable if found, -1 if not.
 */
static int find_existing_var(char **old_env, const char *var,
    size_t var_len)
{
    for (int i = 0; old_env[i]; i++) {
        if (strncmp(old_env[i], var, var_len) == 0 &&
        old_env[i][var_len] == '=')
            return i;
    }
    return -1;
}

/**
 * @brief Creates a new environment with the updated variable.
 *
 * @param old_env : The old environment.
 * @param var : The variable to be added or updated.
 * @param val : The value of the variable.
 * @return : A pointer to the new environment.
 */
static char **create_new_env(char **old_env, const char *var, const char *val)
{
    env_data_t data = {.old_env = old_env, .new_env = NULL, .var = var,
    .val = val, .env_size = 0, .var_len = var ? strlen(var) : 0,
    .val_len = val ? strlen(val) : 0, .var_exists = -1};

    while (data.old_env[data.env_size])
        data.env_size++;
    data.new_env = malloc(sizeof(char *) * (data.env_size + 2));
    if (!data.new_env)
        return NULL;
    data.var_exists = find_existing_var(data.old_env, data.var, data.var_len);
    data.new_env = copy_old_env(&data);
    if (!data.new_env)
        return NULL;
    if (data.var_exists == -1)
        data.new_env = add_new_variable(&data);
    else
        data.new_env[data.env_size] = NULL;
    return data.new_env;
}

/**
 * @brief Implements the setenv command.
 *
 * @param args : Arguments passed to the setenv command.
 * @param count : Number of arguments.
 * @return : 0 on success, 1 on error with message.
 */
int my_setenv(char *args[], int count)
{
    extern char **environ;
    char **new_env;

    if (count == 0) {
        print_environment();
        return 0;
    }
    if (count > 2)
        return print_error(get_error_msg(ERR_TOO_MANY), NULL, 1);
    if (!is_valid_identifier(args[1]))
        return 1;
    new_env = create_new_env(environ, args[1], (count == 2) ? args[2] : "");
    if (!new_env) {
        perror("malloc");
        return 1;
    }
    environ = new_env;
    return 0;
}
