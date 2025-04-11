/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** parsing_args.c
*/

#include "mysh.h"

/**
 * @brief Checks the input and processes the exit or help commands.
 *
 * @param line : The input line read from the user.
 * @param read : The number of characters read.
 */
int check_input(char *line, ssize_t read)
{
    if (read == 0)
        return 1;
    if (line[read - 1] == '\n')
        line[read - 1] = '\0';
    if (strcmp(line, "-h") == 0 || strcmp(line, "--help") == 0) {
        print_help();
        return 0;
    }
    return 0;
}

/**
 * @brief Trims leading spaces and tabs from a line.
 * Moves the pointer forward to skip over leading whitespace.
 *
 * @param line : A pointer to the string to be trimmed.
 */
static void trim_leading_spaces(char **line)
{
    while (**line && (**line == ' ' || **line == '\t'))
        (*line)++;
}

/**
 * @brief Parses a line, allocates memory, and tokenizes arguments.
 *
 * @param line : The string to be parsed.
 * @param args : A pointer to the array of arguments.
 * @param count : A pointer to store the number of arguments.
 * @param status : The status code to return if the line is empty.
 * @return : `status` if the line is empty, 84 on allocation failure,
 * -1 on success.
 */
static int parse_and_allocate_args(char *line, char ***args, int *count,
    int status)
{
    trim_leading_spaces(&line);
    if (*line == '\0') {
        *args = NULL;
        return status;
    }
    *args = tokenize_with_quotes(line);
    if (!*args)
        return 84;
    *count = 0;
    while ((*args)[*count])
        (*count)++;
    return (*count > 0) ? -1 : status;
}

/**
 * @brief Frees allocated arguments and returns a status code.
 *
 * @param args : The argument array to free.
 * @param status : The status code to return.
 * @return : The provided status code.
 */
static int free_return(char **args, int status)
{
    free(args);
    return status;
}

/**
 * @brief Executes a builtin command based on the arguments.
 *
 * @param args : The command arguments.
 * @param count : The number of arguments.
 * @return : Status code of the command execution, -1 if not a builtin.
 */
static int execute_builtin_command(char **args, int count)
{
    int status;

    if (strcmp(args[0], ENV) == 0) {
        print_environment();
        return free_return(args, count);
    }
    if (strcmp(args[0], EXIT) == 0) {
        status = my_exit(args);
        free_args(args);
        exit(status);
    }
    if (strcmp(args[0], CD) == 0)
        return free_return(args, my_cd(args, count));
    if (strcmp(args[0], SETENV) == 0)
        return free_return(args, my_setenv(args, count));
    if (strcmp(args[0], UNSETENV) == 0)
        return free_return(args, my_unsetenv(args));
    return -1;
}

int parse_args(char *line)
{
    int count = 0;
    char **args = NULL;
    int status = 0;
    int builtin_result;
    int parse_status;

    parse_status = parse_and_allocate_args(line, &args, &count, status);
    if (parse_status != -1)
        return parse_status;
    if (count > 0) {
        builtin_result = execute_builtin_command(args, count);
        if (builtin_result != -1)
            return builtin_result;
    }
    status = create_fork(args, count);
    free_args(args);
    return status;
}
