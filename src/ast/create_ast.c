/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** create_ast.c
*/

#include "mysh.h"
#include "ast.h"

/**
 * @brief Creates a command node in the AST.
 *
 * @param args : The command arguments (ownership is transferred).
 * @return : Newly created command node, or NULL on error.
 */
ast_node_t *create_command_node(char **args)
{
    ast_node_t *node = malloc(sizeof(ast_node_t));

    if (!node) {
        perror("malloc failed");
        return NULL;
    }
    node->type = NODE_COMMAND;
    node->args = args;
    node->redirections = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/**
 * @brief Creates an operator node in the AST.
 *
 * @param type : The type of operator (pipe or semicolon).
 * @param left : The left operand node.
 * @param right : The right operand node.
 * @return : Newly created operator node, or NULL on error.
 */
ast_node_t *create_operator_node(node_type_t type, ast_node_t *left,
    ast_node_t *right)
{
    ast_node_t *node;

    if (type != NODE_PIPE && type != NODE_SEMICOLON &&
    type != NODE_AND && type != NODE_OR) {
        print_error("Invalid operator node type\n", NULL, 0);
        return NULL;
    }
    node = malloc(sizeof(ast_node_t));
    if (!node) {
        perror("malloc failed");
        return NULL;
    }
    node->type = type;
    node->args = NULL;
    node->redirections = NULL;
    node->left = left;
    node->right = right;
    return node;
}

/**
 * @brief Checks for conflicting redirections in a node.
 *
 * @param redirections : The current redirections list.
 * @param type : The new redirection type to check.
 * @return : 0 if no conflicts, 1 if conflicts with error message.
 */
static int check_redirection_conflicts(redir_t *redirections, node_type_t type)
{
    redir_t *current = redirections;

    while (current) {
        if ((type == NODE_REDIR_IN || type == NODE_REDIR_HEREDOC) &&
        (current->type == NODE_REDIR_IN ||
        current->type == NODE_REDIR_HEREDOC))
            return print_error(get_error_msg(ERR_AMB_IN_REDIR), NULL, 1);
        if ((type == NODE_REDIR_OUT || type == NODE_REDIR_APPEND) &&
        (current->type == NODE_REDIR_OUT ||
        current->type == NODE_REDIR_APPEND))
            return print_error(get_error_msg(ERR_AMB_OUT_REDIR), NULL, 1);
        current = current->next;
    }
    return 0;
}

/**
 * @brief Creates a redirection structure.
 *
 * @param type : The type of redirection.
 * @param file : The file or content for redirection.
 * @return : Newly created redirection structure, or NULL on error.
 */
static redir_t *create_redirection(node_type_t type, char *file)
{
    redir_t *redir = malloc(sizeof(redir_t));

    if (!redir) {
        perror("malloc failed");
        return NULL;
    }
    redir->type = type;
    redir->file = strdup(file);
    if (!redir->file) {
        perror("strdup failed");
        free(redir);
        return NULL;
    }
    redir->next = NULL;
    return redir;
}

/**
 * @brief Appends a redirection to a node's redirection list.
 *
 * @param node : The AST node to update.
 * @param redir : The redirection to append.
 */
static void append_redirection(ast_node_t *node, redir_t *redir)
{
    redir_t *current;

    if (!node->redirections) {
        node->redirections = redir;
        return;
    }
    current = node->redirections;
    while (current->next)
        current = current->next;
    current->next = redir;
}

/**
 * @brief Adds a redirection to an AST node.
 *
 * @param node : The AST node to update.
 * @param type : The type of redirection.
 * @param file : The file or content for redirection.
 */
void add_redirection(ast_node_t *node, node_type_t type, char *file)
{
    redir_t *redir;

    if (!node || !file || !is_valid_redirection_type(type) ||
    check_redirection_conflicts(node->redirections, type))
        return;
    redir = create_redirection(type, file);
    if (!redir)
        return;
    append_redirection(node, redir);
}

/**
 * @brief Handles errors in heredoc input.
 *
 * @param delimiter : The heredoc delimiter.
 * @param line : The current line buffer (to be freed).
 * @param pipe_fds : The pipe file descriptors to close.
 * @return : -1 to indicate error.
 */
static int handle_heredoc_input_error(char *delimiter, char *line,
    int pipe_fds[2])
{
    free(line);
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    print_error("Warning: unexpected EOF while looking for '", NULL, 0);
    return print_error(delimiter, "'", -1);
}

/**
 * @brief Reads and processes a line for heredoc.
 *
 * @param line : Pointer to line buffer.
 * @param len : Pointer to line buffer size.
 * @param delimiter : The heredoc delimiter.
 * @return : Line length, 0 if delimiter found, -1 on EOF.
 */
static ssize_t read_and_process_line(char **line, size_t *len, char *delimiter)
{
    ssize_t read;

    if (isatty(STDIN_FILENO))
        write(STDOUT_FILENO, "heredoc> ", 9);
    read = getline(line, len, stdin);
    if (read == -1)
        return -1;
    if (read > 0 && (*line)[read - 1] == '\n') {
        (*line)[read - 1] = '\0';
        read--;
    }
    return strcmp(*line, delimiter) == 0 ? 0 : read;
}

/**
 * @brief Implements the heredoc functionality.
 * Reads lines until delimiter is encountered and creates a pipe for the
 * content.
 *
 * @param delimiter : The heredoc delimiter.
 * @return : File descriptor for reading the heredoc content, or -1 on error.
 */
int handle_heredoc(char *delimiter)
{
    int pipe_fds[2];
    char *line = NULL;
    size_t len = 0;
    ssize_t read_len;

    if (pipe(pipe_fds) == -1)
        return -1;
    while (1) {
        read_len = read_and_process_line(&line, &len, delimiter);
        if (read_len == -1)
            return handle_heredoc_input_error(delimiter, line, pipe_fds);
        if (read_len == 0)
            break;
        write(pipe_fds[1], line, read_len);
        write(pipe_fds[1], "\n", 1);
    }
    free(line);
    close(pipe_fds[1]);
    return pipe_fds[0];
}
