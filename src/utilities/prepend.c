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
char *prepend(char *line, char *pre, bool space)
{
    int space_amt = space ? 2 : 1;
    char *str = NULL;

    if (!line || !pre)
        return NULL;
    str = calloc(strlen(pre) + strlen(line) + space_amt, sizeof(char));
    if (!str)
        return NULL;
    strcat(str, pre);
    if (space)
        strcat(str, " ");
    strcat(str, line);
    return str;
}
