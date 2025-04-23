/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** strisdigit.c
*/

#include "mysh.h"

/**
 * @brief Tells us if the string is composed of digits only.
 *
 * @param str : The input string
 * @return 1 if it's the case, 0 if it isn't.
*/
int strisdigit(char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i]) == 0)
            return 0;
    }
    return 1;
}
