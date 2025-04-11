/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_exit.c
*/

#include "mysh.h"

/**
 * @brief Handles the `exit` command.
 * Validates arguments and returns an appropriate exit status.
 *
 * @param args : The array of arguments.
 * @return : The exit status (converted from string) if valid,
 * 84 for argument count error, 1 for invalid input.
 */
int my_exit(char *args[])
{
    if (!args[1])
        return 0;
    if (args[1] && args[2])
        return print_error(get_error_msg(ERR_EXIT_SYNTAX), NULL, 1);
    for (int i = 0; args[1][i]; i++) {
        if (i == 0 && args[1][i] == '-')
            continue;
        if (args[1][i] < '0' || args[1][i] > '9')
            return print_error(get_error_msg(ERR_EXIT_BAD_FORMAT),
            NULL, 1);
    }
    return atoi(args[1]);
}
