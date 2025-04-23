/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** count_lines_file.c
*/

#include "mysh.h"

/**
 * @brief Count lines of a file
 *
 * @param fp : File pointer to the file to read.
 * @return : The amount of lines read.
*/
long count_lines_file(FILE *fp)
{
    long i = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t n = 0;

    if (!fp)
        return -1;
    n = getline(&line, &len, fp);
    while (n != -1) {
        i++;
        n = getline(&line, &len, fp);
    }
    free(line);
    if (fseek(fp, 0, SEEK_SET) == -1)
        return -1;
    return i;
}
