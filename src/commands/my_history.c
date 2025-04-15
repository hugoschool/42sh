/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_history.c
*/

#include "mysh.h"

// TODO: docstrings
int save_history(char *line)
{
    FILE *file = fopen(HISTORY_FILE, "a");
    char *temp = calloc(sizeof(char), strlen(line) + 2);

    if (!file)
        return 0;
    strcat(temp, line);
    strcat(temp, "\n");
    fwrite(temp, sizeof(char), strlen(temp), file);
    free(temp);
    return 1;
}
