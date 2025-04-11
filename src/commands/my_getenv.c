/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_getenv.c
*/

#include "mysh.h"

/**
 * @brief Frees the memory of a new environment array.
 * Iterates through the array and frees each element, then frees the array
 * itself.
 *
 * @param new_env : The new environment array to be freed.
 * @param i : The number of elements in the new environment.
 */
static void free_new_env(char **new_env, int i)
{
    for (int j = 0; j < i; j++)
        free(new_env[i]);
    free(new_env);
}

/**
 * @brief Copies the current environment to a new environment array.
 *
 * @param environ : The current environment array.
 * @param new_env : The new environment array to be filled.
 * @param env_size : The size of the current environment.
 * @return : The new environment array, or NULL if an error occurs.
 */
static char **copy_env(char **environ, char **new_env, int env_size)
{
    if (!new_env)
        return NULL;
    for (int i = 0; environ[i]; i++) {
        new_env[i] = strdup(environ[i]);
        if (!new_env[i]) {
            free_new_env(new_env, i);
            return NULL;
        }
    }
    new_env[env_size] = strdup(NLSPATH_PRINT_PT1 NLSPATH_PRINT_PT2);
    new_env[env_size + 1] = NULL;
    return new_env;
}

/**
 * @brief Adds a specific path (NLSPATH_PT1 NLSPATH_PT2) to the environment
 * and executes a program.
 *
 * @param environ : The current environment array.
 * @param args : The arguments passed to the function, where args[0] is
 * the program to execute.
 */
void add_nlspath(char **environ, char *args[])
{
    char **new_env = NULL;
    int env_size = 0;

    if (strcmp(args[0], ENV_PATH) == 0) {
        while (environ[env_size])
            env_size++;
        new_env = malloc(sizeof(char *) * (env_size + 2));
        new_env = copy_env(environ, new_env, env_size);
        if (!new_env)
            return;
        execve(args[0], args, new_env);
        for (int i = 0; new_env[i]; i++)
            free(new_env[i]);
        free(new_env);
    }
}

/**
 * @brief Retrieves the value of an environment variable
 * Searches through the environment for a variable matching the given name
 * and returns its value.
 *
 * @param name : The name of the environment variable to find
 * @return : The value of the environment variable, or NULL if not found
 */
char *my_getenv(const char *name)
{
    extern char **environ;
    int name_len;

    if (!name || !environ)
        return NULL;
    name_len = strlen(name);
    for (int i = 0; environ[i]; i++) {
        if (strncmp(environ[i], name, name_len) == 0 &&
            environ[i][name_len] == '=') {
            return &environ[i][name_len + 1];
        }
    }
    return NULL;
}
