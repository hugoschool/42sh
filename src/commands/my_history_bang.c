/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_history_bang.c
*/

#include "mysh.h"

// TODO: docstrings
int my_history_bang(char *args[], int count)
{
    if (strcmp(args[0], HISTORY_BANG) == 0)
        return print_error(args[0], get_error_msg(ERR_NOT_FOUND), 1);
    return 0;
}
