/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** executor.c
*/

#include "mysh.h"

/**
 * @brief Processes the command based on the arguments.
 *
 * @param args : Arguments passed to the command.
 * @param count : Number of arguments.
 * @return : Status code of the command execution.
 */
static int get_command(char *args[], int count)
{
    if (strcmp(args[0], CD) == 0)
        return my_cd(args, count);
    if (strcmp(args[0], SETENV) == 0)
        return my_setenv(args, count);
    if (strcmp(args[0], UNSETENV) == 0) {
        if (check_unsetenv(args) == 1)
            return 1;
    }
    execute_command_path(args);
    return 0;
}

/**
 * @brief Handles built-in commands like cd, setenv, and unsetenv.
 *
 * @param args : Arguments passed to the command.
 * @param count : Number of arguments.
 * @return : Status code of the command execution, 0 if not a built-in.
 */
static int handle_builtin_commands(char *args[], int count)
{
    if (strcmp(args[0], CD) == 0 || strcmp(args[0], SETENV) == 0 ||
    strcmp(args[0], UNSETENV) == 0)
        return get_command(args, count);
    return 0;
}

/**
 * @brief Checks the status of a child process and returns the
 * corresponding exit status.
 *
 * @param status : The status code returned by the child process.
 * @return : The exit status of the child process, or an error code if
 * terminated by a signal.
 */
static int check_status(int status)
{
    int signal = WTERMSIG(status);

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    if (WIFSIGNALED(status))
        return print_error(strsignal(signal), NULL, WCOREDUMP(status)) ?
        128 + status : 1;
    return status;
}

/**
 * @brief Executes a command by parsing and processing the command line.
 *
 * @param line : The command line to execute.
 * @return : The status of the command execution.
 */
int main_execute_command(char *line)
{
    ast_node_t *root = parse_line(line);
    int status;

    if (!root)
        return 1;
    status = execute_ast(root);
    free_ast(root);
    return status;
}

/**
 * @brief Creates a child process and executes the command.
 * Creates a fork to execute the command in a child process, handling status.
 *
 * @param args : Arguments passed to the command.
 * @param count : Number of arguments.
 * @return : Status code of the child process execution.
 */
int create_fork(char *args[], int count)
{
    pid_t pid;
    int status;
    int builtin_return = handle_builtin_commands(args, count);

    if (builtin_return != 0)
        return builtin_return;
    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }
    if (pid == 0) {
        execute_command_path(args);
        exit(1);
    }
    waitpid(pid, &status, 0);
    return check_status(status);
}
