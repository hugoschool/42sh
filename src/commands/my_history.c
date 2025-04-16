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

static void read_entire_file(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t n = 0;
    FILE *fp = get_file_path(HISTORY_FILE, "r");

    if (!fp)
        return;
    n = getline(&line, &len, fp);
    while (n != -1) {
        printf("%s", line);
        n = getline(&line, &len, fp);
    }
    fclose(fp);
}

// TODO: docstrings
int my_history(char *args[], int count)
{
    (void) args;
    if (count == 1) {
        read_entire_file();
    }
    return 0;
}
