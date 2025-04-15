/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** concat_args.c
*/

#include "mysh.h"

/**
 * @brief Concatenates the arguments of all arguments seperated by a space.
 *
 * @param args : Arguments given by a NULL terminated char array.
 * @return : A concatenated strings if successful, NULL if not.
 */
char *concat_args(char *args[])
{
    char *str = NULL;
    int str_size = 0;
    int n = -1;

    if (!args)
        return NULL;
    for (int i = 0; args[i] != NULL; i++) {
        n++;
        str_size += strlen(args[i]);
    }
    str = calloc(n + str_size + 1, sizeof(char));
    if (!str)
        return NULL;
    for (int i = 0; i < n + 1; i++) {
        strcat(str, args[i]);
        if (i < n)
            strcat(str, " ");
    }
    return str;
}
