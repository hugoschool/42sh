/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** pipeline_parser.c
*/

#include "mysh.h"

/**
 * @brief Validates that a command follows a pipe operator.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : 1 if valid, 0 if invalid with error message.
 */
static int is_valid_command_after_pipe(char **tokens, int *pos, int max_pos)
{
    if (*pos >= max_pos || !tokens[*pos] ||
    tokens[*pos][0] == PIPE || tokens[*pos][0] == ';')
        return print_error(get_error_msg(ERR_INVALID_NULL_COMMAND),
        NULL, 0);
    return 1;
}

/**
 * @brief Processes the right side command of a pipe.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @param left : The left command AST node.
 * @return : AST node for the right command, or NULL on error.
 */
static ast_node_t *process_right_command(char **tokens, int *pos,
    int max_pos, ast_node_t *left)
{
    ast_node_t *right = parse_command(tokens, pos, max_pos);

    if (!right) {
        print_error(get_error_msg(ERR_INVALID_NULL_COMMAND), NULL, 0);
        free_ast(left);
        return NULL;
    }
    return right;
}

/**
 * @brief Creates a pipe node connecting left and right commands.
 *
 * @param left : The left command AST node.
 * @param right : The right command AST node.
 * @return : AST node for the pipe operation, or NULL on error.
 */
static ast_node_t *create_pipe_node(ast_node_t *left, ast_node_t *right)
{
    ast_node_t *pipe_node = create_operator_node(NODE_PIPE, left, right);

    if (!pipe_node) {
        free_ast(right);
        free_ast(left);
        return NULL;
    }
    return pipe_node;
}

/**
 * @brief Parses a pipeline of commands connected by pipe operators.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : AST node representing the pipeline, or NULL on error.
 */
ast_node_t *parse_pipeline(char **tokens, int *pos, int max_pos)
{
    ast_node_t *left = NULL;
    ast_node_t *right = NULL;

    left = parse_command(tokens, pos, max_pos);
    if (!left)
        return NULL;
    while (*pos < max_pos && tokens[*pos] && tokens[*pos][0] == PIPE) {
        (*pos)++;
        if (!is_valid_command_after_pipe(tokens, pos, max_pos))
            return NULL;
        right = process_right_command(tokens, pos, max_pos, left);
        if (!right)
            return NULL;
        left = create_pipe_node(left, right);
        if (!left)
            return NULL;
    }
    return left;
}
