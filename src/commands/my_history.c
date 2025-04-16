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
    FILE *fp = get_file_path(HISTORY_FILE, "a");
    char *temp = calloc(strlen(line) + 2, sizeof(char));

    if (!fp)
        return 0;
    strcat(temp, line);
    strcat(temp, "\n");
    fwrite(temp, sizeof(char), strlen(temp), fp);
    free(temp);
    fclose(fp);
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
    free(line);
    fclose(fp);
}

// TODO: docstrings
// TODO: if n > lines, print whole file
static void read_n_last_lines(long n)
{
    FILE *fp = get_file_path(HISTORY_FILE, "r");
    long lines = count_lines_file(fp);
    char *line = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    long i = 0;

    if (!fp || lines == -1 || n < 0)
        return;
    while (nread != -1 && i != (lines - n + 1)) {
        nread = getline(&line, &len, fp);
        i++;
    }
    while (nread != -1) {
        printf("%s", line);
        nread = getline(&line, &len, fp);
    }
    free(line);
    fclose(fp);
}

// TODO: docstrings
// TODO: error handling here such as badly formed number
int my_history(char *args[], int count)
{
    if (strncmp(args[0], HISTORY_BANG, strlen(HISTORY_BANG)) == 0)
        return my_history_bang(args, count);
    if (count == 0)
        read_entire_file();
    if (count == 1) {
        if (atol(args[1]) == 0 && strncmp(args[1], "0", 1) != 0)
            return print_error(args[0], get_error_msg(ERR_BAD_FORMAT_NUM), 1);
        read_n_last_lines(atol(args[1]) == 0 ? 1 : atol(args[1]));
    }
    return 0;
}
