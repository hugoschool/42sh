/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** execute_pipe.c
*/

#include "mysh.h"

/**
 * @brief Creates a pipe for command communication.
 *
 * @param pipe_fds : Array to store the pipe file descriptors.
 * @return : 0 on success, 1 on failure.
 */
static int create_pipe(int pipe_fds[2])
{
    if (pipe(pipe_fds) == -1) {
        perror("pipe failed");
        return 1;
    }
    return 0;
}

/**
 * @brief Executes the left side of a pipe.
 *
 * @param pipe_fds : The pipe file descriptors.
 * @param node : The left AST node to execute.
 * @return : Process ID of the left command, -1 on error.
 */
static pid_t execute_left_command(int pipe_fds[2], ast_node_t *node)
{
    pid_t left_pid = fork();
    int status;

    if (left_pid == -1) {
        perror("fork failed");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return -1;
    }
    if (left_pid == 0) {
        signal(SIGINT, SIG_DFL);
        close(pipe_fds[0]);
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[1]);
        status = execute_ast(node->left);
        exit(status);
    }
    return left_pid;
}

/**
 * @brief Executes a builtin command on the right side of a pipe.
 *
 * @param pipe_fds : The pipe file descriptors.
 * @param node : The right AST node to execute.
 * @param left_pid : Process ID of the left command.
 * @param left_status : Pointer to store the left command's status.
 * @return : Status of the right command execution.
 */
static int execute_builtin_right(int pipe_fds[2], ast_node_t *node,
    pid_t left_pid, int *left_status)
{
    int old_stdin = dup(STDIN_FILENO);
    int right_status;

    close(pipe_fds[1]);
    dup2(pipe_fds[0], STDIN_FILENO);
    close(pipe_fds[0]);
    waitpid(left_pid, left_status, 0);
    right_status = execute_ast(node->right);
    dup2(old_stdin, STDIN_FILENO);
    close(old_stdin);
    return right_status;
}

/**
 * @brief Executes the right side of a pipe.
 *
 * @param pipe_fds : The pipe file descriptors.
 * @param node : The right AST node to execute.
 * @param left_pid : Process ID of the left command.
 * @return : Process ID of the right command, -1 on error.
 */
static pid_t execute_right_command(int pipe_fds[2], ast_node_t *node,
    pid_t left_pid)
{
    pid_t right_pid = fork();
    int status;

    if (right_pid == -1) {
        perror("fork failed");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        kill(left_pid, SIGTERM);
        waitpid(left_pid, NULL, 0);
        return -1;
    }
    if (right_pid == 0) {
        signal(SIGINT, SIG_DFL);
        close(pipe_fds[1]);
        dup2(pipe_fds[0], STDIN_FILENO);
        close(pipe_fds[0]);
        status = execute_ast(node->right);
        exit(status);
    }
    return right_pid;
}

/**
 * @brief Waits for both processes and processes their exit status.
 *
 * @param left_pid : Process ID of the left command.
 * @param right_pid : Process ID of the right command.
 * @param left_status : Pointer to store the left command's status.
 * @param right_status : Pointer to store the right command's status.
 * @return : Combined exit status of the commands.
 */
static int wait_and_proc_status(pid_t left_pid, pid_t right_pid,
    int *left_status, int *right_status)
{
    int signal;

    waitpid(left_pid, left_status, 0);
    waitpid(right_pid, right_status, 0);
    if (WIFEXITED(*right_status))
        return WEXITSTATUS(*right_status);
    if (WIFSIGNALED(*right_status)) {
        signal = WTERMSIG(*right_status);
        return print_error(strsignal(signal),
        WCOREDUMP(*right_status) ? CORE_DUMPED : "", 128 + signal);
    }
    return 1;
}

/**
 * @brief Checks if a node represents a builtin command.
 *
 * @param node : The AST node to check.
 * @return : 1 if the node is a builtin command, 0 otherwise.
 */
static int is_builtin_command(ast_node_t *node)
{
    if (!node || !node->args || !node->args[0])
        return 0;
    return (strcmp(node->args[0], CD) == 0 ||
    strcmp(node->args[0], EXIT) == 0 ||
    strcmp(node->args[0], SETENV) == 0 ||
    strcmp(node->args[0], UNSETENV) == 0 ||
    strcmp(node->args[0], ENV) == 0);
}

/**
 * @brief Executes a pipe expression with a left and right command.
 *
 * @param node : The pipe node to execute.
 * @return : Status of the pipe execution.
 */
int execute_pipe(ast_node_t *node)
{
    int pipe_fds[2];
    pid_t left_pid;
    pid_t right_pid;
    int left_stts = 0;
    int right_stts = 0;
    int builtin_right = is_builtin_command(node->right);

    if (create_pipe(pipe_fds) != 0)
        return 1;
    left_pid = execute_left_command(pipe_fds, node);
    if (left_pid == -1)
        return 1;
    if (builtin_right)
        return execute_builtin_right(pipe_fds, node, left_pid, &left_stts);
    right_pid = execute_right_command(pipe_fds, node, left_pid);
    if (right_pid == -1)
        return 1;
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    return wait_and_proc_status(left_pid, right_pid, &left_stts, &right_stts);
}
