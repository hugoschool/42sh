/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** command_parser.c
*/

#include "mysh.h"

/**
 * @brief Counts the number of consecutive command arguments.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : Number of command arguments found.
 */
static int count_command_arguments(char **tokens, int *pos, int max_pos)
{
    int count = 0;

    while (*pos < max_pos && tokens[*pos] && tokens[*pos][0] != ';' &&
    tokens[*pos][0] != PIPE && tokens[*pos][0] != REDIR_IN &&
    tokens[*pos][0] != REDIR_OUT) {
        count++;
        (*pos)++;
    }
    return count;
}

/**
 * @brief Allocates memory for an array to store command arguments.
 *
 * @param count : Number of arguments to allocate space for.
 * @return : Allocated array, or NULL on error.
 */
static char **allocate_arguments(int count)
{
    char **args = malloc(sizeof(char *) * (count + 1));

    if (!args) {
        perror("malloc failed");
        return NULL;
    }
    return args;
}

/**
 * @brief Copies tokens into an arguments array.
 *
 * @param args : Destination array for arguments.
 * @param tokens : Source array of tokens.
 * @param start : Starting index in tokens.
 * @param count : Number of arguments to copy.
 * @return : 0 on success, -1 on error.
 */
static int copy_arguments(char **args, char **tokens, int start, int count)
{
    for (int i = 0; i < count; i++) {
        args[i] = strdup(tokens[start + i]);
        if (!args[i]) {
            free_args(args);
            return -1;
        }
        strip_quotes(args[i]);
    }
    args[count] = NULL;
    return 0;
}

/**
 * @brief Determines the redirection type from a token.
 *
 * @param token : The token to check.
 * @return : The corresponding node type for the redirection.
 */
static node_type_t get_redirection_type(const char *token)
{
    if (strcmp(token, ">") == 0)
        return NODE_REDIR_OUT;
    if (strcmp(token, "<") == 0)
        return NODE_REDIR_IN;
    if (strcmp(token, ">>") == 0)
        return NODE_REDIR_APPEND;
    if (strcmp(token, "<<") == 0)
        return NODE_REDIR_HEREDOC;
    return NONE;
}

/**
 * @brief Processes a redirection token and updates the command node.
 *
 * @param cmd_node : The command node to update.
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : 0 on success, -1 on error with message.
 */
static int process_redirection(ast_node_t *cmd_node, char **tokens,
    int *pos, int max_pos)
{
    node_type_t redir_type;
    char *file_copy;

    redir_type = get_redirection_type(tokens[*pos]);
    if (redir_type == NONE)
        return -1;
    (*pos)++;
    if (*pos >= max_pos || !tokens[*pos])
        return print_error(NULL, get_error_msg(ERR_NO_NAME_REDIRECTION), -1);
    file_copy = strdup(tokens[*pos]);
    if (!file_copy)
        return -1;
    strip_quotes(file_copy);
    add_redirection(cmd_node, redir_type, file_copy);
    free(file_copy);
    (*pos)++;
    return 0;
}

/**
 * @brief Handles all redirections for a command.
 *
 * @param cmd_node : The command node to update.
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : 0 on success, -1 on error.
 */
static int handle_redirections(ast_node_t *cmd_node, char **tokens,
    int *pos, int max_pos)
{
    while (*pos < max_pos && tokens[*pos] &&
    (tokens[*pos][0] == REDIR_IN || tokens[*pos][0] == REDIR_OUT)) {
        if (process_redirection(cmd_node, tokens, pos, max_pos) == -1) {
            free_ast(cmd_node);
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Parses a single command from tokens.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : AST node for the command, or NULL on error.
 */
ast_node_t *parse_command(char **tokens, int *pos, int max_pos)
{
    int start = *pos;
    int count = 0;
    char **args = NULL;
    ast_node_t *cmd_node = NULL;

    count = count_command_arguments(tokens, pos, max_pos);
    if (count == 0)
        return NULL;
    args = allocate_arguments(count);
    if (!args || copy_arguments(args, tokens, start, count) == -1)
        return NULL;
    cmd_node = create_command_node(args);
    if (!cmd_node) {
        free_args(args);
        return NULL;
    }
    if (handle_redirections(cmd_node, tokens, pos, max_pos) == -1)
        return NULL;
    return cmd_node;
}

/**
 * @brief Processes a sequence of semicolon-separated commands.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @param left : The left-most command AST node.
 * @return : AST node representing the command list, or NULL on error.
 */
static ast_node_t *process_semicolon_nodes(char **tokens, int *pos,
    int max_pos, ast_node_t *left)
{
    ast_node_t *right;
    ast_node_t *semicolon_node;

    while (*pos < max_pos && tokens[*pos] && !strcmp(tokens[*pos], ";")) {
        (*pos)++;
        while (*pos < max_pos && tokens[*pos] && !strcmp(tokens[*pos], ";"))
            (*pos)++;
        if (*pos >= max_pos || !tokens[*pos])
            break;
        right = parse_pipeline(tokens, pos, max_pos);
        if (!right)
            continue;
        semicolon_node = create_operator_node(NODE_SEMICOLON, left, right);
        if (!semicolon_node) {
            free_ast(right);
            return (ast_node_t *)free_ast(left);
        }
        left = semicolon_node;
    }
    return left;
}

/**
 * @brief Parses a list of commands separated by semicolons.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : AST node representing the command list, or NULL on error.
 */
ast_node_t *parse_command_list(char **tokens, int *pos, int max_pos)
{
    ast_node_t *left;

    while (*pos < max_pos && tokens[*pos] && !strcmp(tokens[*pos], ";"))
        (*pos)++;
    if (*pos >= max_pos || !tokens[*pos])
        return NULL;
    left = parse_pipeline(tokens, pos, max_pos);
    if (!left)
        return NULL;
    return process_semicolon_nodes(tokens, pos, max_pos, left);
}
