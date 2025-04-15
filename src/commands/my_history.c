/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_history.c
*/

#include "mysh.h"

// TODO: docstrings
// TODO: change open in home to create file
int save_history(char *line)
{
    FILE *fd = get_file_path(HISTORY_FILE, "a");
    char *temp = calloc(strlen(line) + 2, sizeof(char));

    if (!fd)
        return 0;
    strcat(temp, line);
    strcat(temp, "\n");
    fwrite(temp, sizeof(char), strlen(temp), fd);
    free(temp);
    return 1;
}
