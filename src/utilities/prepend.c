/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** prepend.c
*/

#include "mysh.h"

/**
 * @brief Puts a string before another.
 *
 * @param pre : The string you want to come before.
 * @param line : The rest of the string.
 * @return : Allocated string if successful, NULL if not.
 */
char *prepend(char *line, char *pre)
{
    char *str = calloc(strlen(pre) + strlen(line) + 1, sizeof(char));

    if (!str)
        return NULL;
    strcat(str, pre);
    strcat(str, line);
    return str;
}
