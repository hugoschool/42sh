/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** logical_parser.c
*/

#include "mysh.h"

/**
 * @brief Handles logical operators (&&, ||) during tokenization.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @param i : Current position in the line.
 * @return : 0 on success, -1 on failure, 1 for double operator.
 */
int handle_logical_operator(token_line_t *tl, token_state_t *state,
    int i)
{
    int ret = 0;

    if (!is_logical_op(tl->line, i))
        return 0;
    if (state->in_token) {
        ret = save_current_token(tl, state, i);
        if (ret == -1)
            return -1;
        state->in_token = 0;
    }
    tl->tokens[state->count] = malloc(3);
    if (!tl->tokens[state->count])
        return -1;
    tl->tokens[state->count][0] = tl->line[i];
    tl->tokens[state->count][1] = tl->line[i];
    tl->tokens[state->count][2] = '\0';
    state->count++;
    return 1;
}

/**
 * @brief Validates that a command follows a logical operator (&&, ||).
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @param op_type : The operator type (NODE_AND or NODE_OR).
 * @return : 1 if valid, 0 if invalid with error message.
 */
static int is_valid_command_after_logical(char **tokens, int *pos,
    int max_pos, node_type_t op_type)
{
    if (*pos >= max_pos || !tokens[*pos] || tokens[*pos][0] == PIPE ||
    tokens[*pos][0] == ';' || strcmp(tokens[*pos], AND_OP) == 0 ||
    strcmp(tokens[*pos], OR_OP) == 0) {
        if (op_type == NODE_AND)
            return print_error(get_error_msg(ERR_INVALID_AND_SYNTAX), NULL, 0);
        if (op_type != NODE_OR)
            return print_error(get_error_msg(ERR_INVALID_OR_SYNTAX), NULL, 0);
    }
    return 1;
}

/**
 * @brief Processes the right side command of a logical operator.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @param left : The left command AST node.
 * @param op_type : The operator type (NODE_AND or NODE_OR).
 * @return : AST node for the right command, or NULL on error.
 */
static ast_node_t *process_right_logical_command(ast_node_t *right,
    ast_node_t *left, node_type_t op_type)
{
    if (!right) {
        if (op_type == NODE_AND)
            print_error(get_error_msg(ERR_INVALID_AND_SYNTAX), NULL, 0);
        if (op_type != NODE_AND)
            print_error(get_error_msg(ERR_INVALID_OR_SYNTAX), NULL, 0);
        return (ast_node_t *)free_ast(left);
    }
    return right;
}

/**
 * @brief Creates a logical operator node connecting left and right commands.
 *
 * @param op_type : The operator type (NODE_AND or NODE_OR).
 * @param left : The left command AST node.
 * @param right : The right command AST node.
 * @return : AST node for the logical operation, or NULL on error.
 */
static ast_node_t *create_logical_node(node_type_t op_type, ast_node_t *left,
    ast_node_t *right)
{
    ast_node_t *logical_node = create_operator_node(op_type, left, right);

    if (!logical_node) {
        free_ast(right);
        return (ast_node_t *)free_ast(left);
    }
    return logical_node;
}

/**
 * @brief Parses logical expressions (commands connected by && or ||).
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : AST node representing the logical expression, or NULL on error.
 */
ast_node_t *parse_logical_operator(char **tokens, int *pos, int max_pos,
    ast_node_t *left)
{
    ast_node_t *right = NULL;
    node_type_t op_type = (strcmp(tokens[*pos], AND_OP) == 0) ? NODE_AND :
    NODE_OR;

    (*pos)++;
    if (!is_valid_command_after_logical(tokens, pos, max_pos, op_type))
        return (ast_node_t *)free_ast(left);
    right = parse_pipeline(tokens, pos, max_pos);
    right = process_right_logical_command(right, left, op_type);
    if (!right)
        return (ast_node_t *)free_ast(left);
    return create_logical_node(op_type, left, right);
}

ast_node_t *parse_logical_expression(char **tokens, int *pos, int max_pos)
{
    ast_node_t *left = NULL;

    left = parse_pipeline(tokens, pos, max_pos);
    if (!left)
        return NULL;
    while (*pos < max_pos && tokens[*pos] &&
    (strcmp(tokens[*pos], AND_OP) == 0 || strcmp(tokens[*pos], OR_OP) == 0)) {
        left = parse_logical_operator(tokens, pos, max_pos, left);
        if (!left)
            return NULL;
    }
    return left;
}

/**
 * @brief Executes a logical AND/OR expression with short-circuit evaluation.
 *
 * @param node : The AND/OR operator node to execute.
 * @param op_is_and : && or || operator.
 * @return : Status code from the last executed command.
 */
int execute_logical(ast_node_t *node, int op_is_and)
{
    int left_status;

    if (!node || !node->left || !node->right)
        return print_error(get_error_msg(ERR_INVALID_OPERATOR), NULL, 1);
    left_status = execute_ast(node->left);
    if (op_is_and)
        return (left_status == 0) ? execute_ast(node->right) : left_status;
    return (left_status != 0) ? execute_ast(node->right) : left_status;
}
