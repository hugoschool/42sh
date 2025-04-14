/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** setup_redirection.c
*/

#include "mysh.h"

/**
 * @brief Sets up an input redirection.
 *
 * @param redir : The redirection structure.
 * @return : 0 on success, 1 on error with message.
 */
static int handle_input_redirection(redir_t *redir)
{
    int fd = open(redir->file, O_RDONLY);

    if (fd == -1)
        return print_error(redir->file, get_error_msg(ERR_NO_SUCH_FILE),
        1);
    dup2(fd, STDIN_FILENO);
    close(fd);
    return 0;
}

/**
 * @brief Sets up an output redirection.
 *
 * @param redir : The redirection structure.
 * @return : 0 on success, 1 on error with message.
 */
static int handle_output_redirection(redir_t *redir)
{
    int flags = O_WRONLY | O_CREAT;
    int fd;

    if (redir->type == NODE_REDIR_OUT)
        flags |= O_TRUNC;
    else
        flags |= O_APPEND;
    fd = open(redir->file, flags, 0644);
    if (fd == -1) {
        if (errno == EACCES || errno == EISDIR)
            return print_error(redir->file, get_error_msg(ERR_PERMISSION),
            1);
        return print_error(redir->file,
        get_error_msg(ERR_CANT_CREATE_FILE), 1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return 0;
}

/**
 * @brief Sets up an append redirection.
 *
 * @param redir : The redirection structure.
 * @return : 0 on success, 1 on error with message.
 */
static int handle_append_redirection(redir_t *redir)
{
    int fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1) {
        if (errno == EACCES || errno == EISDIR)
            return print_error(redir->file, get_error_msg(ERR_PERMISSION),
            1);
        return print_error(redir->file,
        get_error_msg(ERR_CANT_CREATE_FILE), 1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return 0;
}

/**
 * @brief Sets up a heredoc redirection.
 *
 * @param redir : The redirection structure.
 * @return : 0 on success, 1 on error with message.
 */
static int handle_heredoc_redirection(redir_t *redir)
{
    int fd = handle_heredoc(redir->file);

    if (fd == -1)
        return print_error(HEREDOC, get_error_msg(ERR_HEREDOC_FAILED), 1);
    dup2(fd, STDIN_FILENO);
    close(fd);
    return 0;
}

/**
 * @brief Sets up all redirections for a command.
 *
 * @param node : The AST node with redirections.
 * @return : 0 on success, or error code on failure.
 */
int setup_redirections(ast_node_t *node)
{
    int status = 0;

    for (redir_t *redir = node->redirections; redir; redir = redir->next) {
        if (redir->type == NODE_REDIR_IN)
            status = handle_input_redirection(redir);
        if (redir->type == NODE_REDIR_OUT)
            status = handle_output_redirection(redir);
        if (redir->type == NODE_REDIR_APPEND)
            status = handle_append_redirection(redir);
        if (redir->type == NODE_REDIR_HEREDOC)
            status = handle_heredoc_redirection(redir);
        if (status != 0)
            return status;
    }
    return 0;
}

/**
 * @brief Restores the original file descriptors after redirection.
 *
 * @param old_stdin : The original stdin file descriptor.
 * @param old_stdout : The original stdout file descriptor.
 */
void restore_redirections(int old_stdin, int old_stdout)
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
