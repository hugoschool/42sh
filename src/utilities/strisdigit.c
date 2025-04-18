/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** strisdigit.c
*/

#include "mysh.h"

// TODO: docstrings
int strisdigit(char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i]) == 0)
            return 0;
    }
    return 1;
}
