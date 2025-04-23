/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_history_bang.c
*/

#include "mysh.h"

/**
 * @brief Read the file and store the line and the previous line
 *
 * @param line : A pointer to the line string.
 * @param prev : A pointer to the previous string.
 * @param command : The command to run.
 * @param fp : File pointer to the file to read.
 * @return : The amount of lines read.
*/
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

/**
 * @brief Execute the last command for the given command
 *
 * @param command : The command to run.
 * @param rest : A concatenate of all given arguments.
 * @return : Exit code of the last given command.
*/
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
        if ((line && strcmp(command, HISTORY_BANG) == 0) || prev) {
            run_command = prepend(rest, prev ? prev : line, ' ');
            printf("%s\n", run_command);
            return main_execute_command(run_command);
        } else
            return print_error(command, get_error_msg(ERR_EVENT_NOT_FOUND), 1);
    }
    return 1;
}

/**
 * @brief The builtin command for history bangs `!`.
 *
 * @param args : A NULL terminated arguments array.
 * @param count : The amount of arguments in the arguments array.
 * @return : Exit code for that builtin.
*/
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
