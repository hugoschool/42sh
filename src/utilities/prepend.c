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
 * @param middle : In case you want to insert a char in the middle,
 * put '\0' if not.
 * @return : Allocated string if successful, NULL if not.
 */
char *prepend(char *line, char *pre, char middle)
{
    int middle_amt = middle != 0 ? 2 : 1;
    char *str = NULL;

    if (!line || !pre)
        return NULL;
    str = calloc(strlen(pre) + strlen(line) + middle_amt, sizeof(char));
    if (!str)
        return NULL;
    strcat(str, pre);
    if (middle != 0)
        strcat(str, (char[]){middle, 0});
    strcat(str, line);
    return str;
}
