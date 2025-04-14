/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** execute_builtins.c
*/

#include "mysh.h"

/**
 * @brief Checks if a command is a builtin that is recognized but not
 * implemented.
 *
 * @param cmd : The command name to check.
 * @return : 1 if it's an unimplemented builtin, 0 otherwise.
 */
static int is_unimplemented_builtin(char *cmd)
{
    const char *unimplemented_builtins[] = {
        "history", "pushd", "popd", "dirs", "set", NULL
    };

    for (int i = 0; unimplemented_builtins[i]; i++) {
        if (strcmp(cmd, unimplemented_builtins[i]) == 0)
            return 1;
    }
    return 0;
}

/**
 * @brief Handles execution of MOST builtin commands (all except exit).
 *
 * @param args : Arguments of the command
 * @param args_count : Amount of arguments
 * @return : Status of the command execution, or -1 if not a builtin.
 */
static int handle_most_builtins(char **args, int arg_count)
{
    if (strcmp(args[0], ALIAS) == 0)
        return my_alias(args, arg_count);
    if (strcmp(args[0], CD) == 0)
        return my_cd(args, arg_count);
    if (strcmp(args[0], ENV) == 0)
        return print_environment();
    if (strcmp(args[0], SETENV) == 0)
        return my_setenv(args, arg_count);
    if (strcmp(args[0], UNSETENV) == 0)
        return my_unsetenv(args);
    return -1;
}

/**
 * @brief Handles execution of builtin commands.
 *
 * @param node : The AST node containing the command.
 * @param old_stdin : The original stdin file descriptor.
 * @param old_stdout : The original stdout file descriptor.
 * @return : Status of the command execution, or -1 if not a builtin.
 */
int handle_builtin_command(ast_node_t *node, int old_stdin, int old_stdout)
{
    int arg_count = count_args(node->args) - 1;
    int status;
    int most_builtins = handle_most_builtins(node->args, arg_count);

    if (most_builtins != -1)
        return most_builtins;
    if (strcmp(node->args[0], EXIT) == 0) {
        status = my_exit(node->args);
        if (node->redirections)
            restore_redirections(old_stdin, old_stdout);
        exit(status);
    }
    if (is_unimplemented_builtin(node->args[0]))
        return 0;
    return -1;
}
