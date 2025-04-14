/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** env.c
*/

#include "mysh.h"

/**
 * @brief Prints the current environment variables.
 *
 * @return : 0 on success.
 */
int print_environment(void)
{
    extern char **environ;

    if (!environ)
        return 0;
    if (!my_getenv(NLSPATH))
        write(1, NLSPATH_PRINT_PT1 NLSPATH_PRINT_PT2, 80);
    for (int i = 0; environ[i]; i++) {
        write(1, environ[i], strlen(environ[i]));
        write(1, "\n", 1);
    }
    return 0;
}
