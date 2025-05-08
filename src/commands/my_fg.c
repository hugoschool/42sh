/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_fg.c
*/

#include "mysh.h"

int my_fg(char *args[], int ac)
{
    if (!args[1])
        return 0;
    if (args[1] && args[2])
        return print_error(get_error_msg(ERR_EXIT_SYNTAX), NULL, 1);
    for (int i = 0; args[1][i]; i++) {
        if (i == 0 && args[1][i] == '-')
            continue;
        if (args[1][i] < '0' || args[1][i] > '9')
            return print_error(args[0],
            get_error_msg(ERR_BAD_FORMAT_NUM), 1);
    }
    return atoi(args[1]);
}
