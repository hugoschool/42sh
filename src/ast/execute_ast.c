/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** executor_ast.c
*/

#include "mysh.h"

/**
 * @brief Recursively executes an AST node.
 *
 * @param node : The AST node to execute.
 * @return : Exit status of the execution.
 */
int execute_ast(ast_node_t *node)
{
    if (!node)
        return 0;
    switch (node->type) {
        case NODE_COMMAND:
            return execute_command(node);
        case NODE_PIPE:
            return execute_pipe(node);
        case NODE_AND:
            return execute_logical(node, 1);
        case NODE_OR:
            return execute_logical(node, 0);
        case NODE_SEMICOLON:
            execute_ast(node->left);
            if (!node->right)
                return 0;
            return execute_ast(node->right);
        default:
            return print_error(get_error_msg(ERR_INVALID_OPERATOR), NULL, 1);
    }
}

/**
 * @brief Counts the number of arguments in an array.
 *
 * @param args : The array of arguments to count.
 * @return : Number of arguments in the array.
 */
static int count_args(char **args)
{
    int count = 0;

    if (!args)
        return 0;
    while (args[count])
        count++;
    return count;
}

/**
 * @brief Restores the original file descriptors after redirection.
 *
 * @param old_stdin : The original stdin file descriptor.
 * @param old_stdout : The original stdout file descriptor.
 */
static void restore_redirections(int old_stdin, int old_stdout)
{
    if (old_stdin != -1) {
        dup2(old_stdin, STDIN_FILENO);
        close(old_stdin);
    }
    if (old_stdout != -1) {
        dup2(old_stdout, STDOUT_FILENO);
        close(old_stdout);
    }
}

/**
 * @brief Checks if a command is a builtin that is recognized but not
 * implemented.
 *
 * @param cmd : The command name to check.
 * @return : 1 if it's an unimplemented builtin, 0 otherwise.
 */
static int is_unimplemented_builtin(char *cmd)
{
    static const char *unimplemented_builtins[] = {
        "alias", "history", "pushd", "popd", "dirs", "set", NULL
    };

    for (int i = 0; unimplemented_builtins[i]; i++) {
        if (strcmp(cmd, unimplemented_builtins[i]) == 0)
            return 1;
    }
    return 0;
}

/**
 * @brief Sets up file descriptors for redirections and checks for errors.
 *
 * @param node : The AST node with redirections.
 * @param old_stdin : Pointer to store the original stdin file descriptor.
 * @param old_stdout : Pointer to store the original stdout file descriptor.
 * @return : 0 on success, 1 on error.
 */
static int setup_and_check_redirections(ast_node_t *node, int *old_stdin,
    int *old_stdout)
{
    *old_stdin = dup(STDIN_FILENO);
    *old_stdout = dup(STDOUT_FILENO);
    if (*old_stdin == -1 || *old_stdout == -1) {
        perror("dup failed");
        return 1;
    }
    if (setup_redirections(node) != 0) {
        restore_redirections(*old_stdin, *old_stdout);
        return 1;
    }
    return 0;
}

/**
 * @brief Handles execution of builtin commands.
 *
 * @param node : The AST node containing the command.
 * @param old_stdin : The original stdin file descriptor.
 * @param old_stdout : The original stdout file descriptor.
 * @return : Status of the command execution, or -1 if not a builtin.
 */
static int handle_builtin_command(ast_node_t *node, int old_stdin,
    int old_stdout)
{
    int arg_count = count_args(node->args) - 1;
    int status;

    if (strcmp(node->args[0], CD) == 0)
        return my_cd(node->args, arg_count);
    if (strcmp(node->args[0], EXIT) == 0) {
        status = my_exit(node->args);
        if (node->redirections)
            restore_redirections(old_stdin, old_stdout);
        exit(status);
    }
    if (strcmp(node->args[0], ENV) == 0)
        return print_environment();
    if (strcmp(node->args[0], SETENV) == 0)
        return my_setenv(node->args, arg_count);
    if (strcmp(node->args[0], UNSETENV) == 0)
        return my_unsetenv(node->args);
    if (is_unimplemented_builtin(node->args[0]))
        return 0;
    return -1;
}

/**
 * @brief Executes a command in a child process.
 *
 * @param node : The AST node containing the command.
 * @return : Exit status of the execution.
 */
static int execute_child_process(ast_node_t *node)
{
    signal(SIGINT, SIG_DFL);
    execute_command_path(node->args);
    return print_error(node->args[0], get_error_msg(ERR_NOT_FOUND), 1);
}

/**
 * @brief Processes the wait status of a child process.
 *
 * @param wait_status : The status returned by waitpid.
 * @return : Normalized exit status.
 */
static int handle_wait_status(int wait_status)
{
    int signal;

    if (WIFEXITED(wait_status))
        return WEXITSTATUS(wait_status);
    if (WIFSIGNALED(wait_status)) {
        signal = WTERMSIG(wait_status);
        return print_error(strsignal(signal),
        WCOREDUMP(wait_status) ? CORE_DUMPED : "", 128 + signal);
    }
    return 1;
}

/**
 * @brief Executes an external (non-builtin) command.
 *
 * @param node : The AST node containing the command.
 * @param old_stdin : The original stdin file descriptor.
 * @param old_stdout : The original stdout file descriptor.
 * @return : Status of the command execution.
 */
static int execute_external_command(ast_node_t *node, int old_stdin,
    int old_stdout)
{
    pid_t pid;
    int wait_status;
    int status;

    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        if (node->redirections)
            restore_redirections(old_stdin, old_stdout);
        return 1;
    }
    if (pid == 0)
        exit(execute_child_process(node));
    waitpid(pid, &wait_status, 0);
    status = handle_wait_status(wait_status);
    return status;
}

/**
 * @brief Executes a command node from the AST.
 *
 * @param node : The command node to execute.
 * @return : Status of the command execution.
 */
int execute_command(ast_node_t *node)
{
    int old_stdin = -1;
    int old_stdout = -1;
    int status = 0;

    if (!node || !node->args || !node->args[0])
        return 0;
    if (node->redirections) {
        if (setup_and_check_redirections(node, &old_stdin, &old_stdout) != 0)
            return 1;
    }
    status = handle_builtin_command(node, old_stdin, old_stdout);
    if (status == -1)
        status = execute_external_command(node, old_stdin, old_stdout);
    if (node->redirections)
        restore_redirections(old_stdin, old_stdout);
    return status;
}
