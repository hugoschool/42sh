/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_history_bang.c
*/

#include "mysh.h"

static int read_file(char **line, char **prev, char *command, FILE *fp)
{
    size_t len = 0;
    ssize_t n = 0;
    int i = 0;

    n = getline(line, &len, fp);
    while (n != -1) {
        if (n > 0 && (*line)[n - 1] == '\n')
            (*line)[n - 1] = '\0';
        if (strcmp(command, HISTORY_BANG) != 0
            && strncmp(*line, command, strlen(command)) == 0)
            *prev = strdup(*line);
        i++;
        n = getline(line, &len, fp);
    }
    return i;
}

static int execute_last_command(char *command, char *rest)
{
    char *line = NULL;
    FILE *fp = get_file_path(HISTORY_FILE, "r");
    char *prev = NULL;
    char *run_command = NULL;
    int i = 0;

    if (!fp || !command || !rest)
        return 1;
    i = read_file(&line, &prev, command, fp);
    fclose(fp);
    if (i != 0) {
        run_command = prepend(rest, prev ? prev : line, true);
        printf("%s\n", run_command);
        return main_execute_command(run_command);
    }
    return 1;
}

// TODO: docstrings
int my_history_bang(char *args[], int count)
{
    char *start = args[0];

    (void) count;
    if (strcmp(args[0], HISTORY_BANG) == 0)
        return print_error(args[0], get_error_msg(ERR_NOT_FOUND), 1);
    start++;
    if (start[0] != '\0')
        return execute_last_command(start, concat_args(&args[1]));
    return 0;
}
