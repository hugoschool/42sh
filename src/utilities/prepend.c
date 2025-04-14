/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** prepend.c
*/

#include "mysh.h"

// TODO: docstrings
char *prepend(char *line, char *pre)
{
    char *str = calloc(sizeof(char), strlen(pre) + strlen(line) + 1);

    if (!str)
        return NULL;
    strcat(str, pre);
    strcat(str, line);
    return str;
}
