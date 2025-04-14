/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** validate_syntax.c
*/

#include "mysh.h"

/**
 * Checks if subshell node is valid
 *
 * @param node The AST node to check
 * @return 0 if valid, error code otherwise
 */
int validate_subshell_node(ast_node_t *node)
{
    if (!node || !node->left)
        return print_error(get_error_msg(ERR_INVALID_SUBSHELL), NULL, 1);
    return 0;
}

/**
 * @brief Validates the balance of parentheses in a token array
 *
 * @param tokens Array of tokens to validate
 * @return int 0 if balanced, error code otherwise
 */
static int validate_parentheses_balance(char **tokens)
{
    int paren_count = 0;
    int i = 0;

    while (tokens[i]) {
        if (tokens[i][0] == OPEN_PAREN)
            paren_count++;
        if (tokens[i][0] != OPEN_PAREN && tokens[i][0] == CLOSE_PAREN)
            paren_count--;
        if (paren_count < 0)
            return print_error(get_error_msg(ERR_MISSING_OPENING_PAREN),
            NULL, 84);
        i++;
    }
    if (paren_count > 0)
        return print_error(get_error_msg(ERR_MISSING_CLOSING_PAREN), NULL, 84);
    return 0;
}

/**
 * @brief Validates if the first token is valid
 *
 * @param tokens Array of tokens to validate
 * @return int 0 if valid, error code otherwise
 */
static int validate_first_token(char **tokens)
{
    if (!tokens || !tokens[0])
        return 0;
    if (tokens[0][0] == PIPE)
        return print_error(get_error_msg(ERR_INVALID_NULL_COMMAND), NULL, 84);
    if (strcmp(tokens[0], AND_OP) == 0 || strcmp(tokens[0], OR_OP) == 0)
        return print_error(get_error_msg(ERR_INVALID_NULL_COMMAND), NULL, 84);
    return 0;
}

/**
 * @brief Validates if consecutive pipes are present
 *
 * @param tokens Array of tokens to validate
 * @param i Current position in tokens array
 * @return int 0 if valid, error code otherwise
 */
static int validate_consecutive_pipes(char **tokens, int i)
{
    if (tokens[i][0] == PIPE && tokens[i + 1] && tokens[i + 1][0] == PIPE)
        return print_error(get_error_msg(ERR_INVALID_NULL_COMMAND), NULL, 84);
    return 0;
}

/**
 * @brief Validates logical operators (AND, OR) syntax
 *
 * @param tokens Array of tokens to validate
 * @param i Current position in tokens array
 * @return int 0 if valid, error code otherwise
 */
static int validate_logical_operators(char **tokens, int i)
{
    if ((strcmp(tokens[i], AND_OP) == 0 || strcmp(tokens[i], OR_OP) == 0) &&
    tokens[i + 1] && (strcmp(tokens[i + 1], AND_OP) == 0 ||
    strcmp(tokens[i + 1], OR_OP) == 0 || tokens[i + 1][0] == PIPE ||
    tokens[i + 1][0] == ';'))
        return print_error(get_error_msg((strcmp(tokens[i], AND_OP) == 0) ?
        ERR_INVALID_AND_SYNTAX : ERR_INVALID_OR_SYNTAX), NULL, 84);
    return 0;
}

/**
 * @brief Validates redirection syntax
 *
 * @param tokens Array of tokens to validate
 * @param i Current position in tokens array
 * @return int 0 if valid, error code otherwise
 */
static int validate_redirections(char **tokens, int i)
{
    if ((tokens[i][0] == REDIR_IN || tokens[i][0] == REDIR_OUT) &&
    (!tokens[i + 1] || tokens[i + 1][0] == PIPE || tokens[i + 1][0] == ';'
    || tokens[i + 1][0] == REDIR_IN || tokens[i + 1][0] == REDIR_OUT ||
    strcmp(tokens[i + 1], AND_OP) == 0 || strcmp(tokens[i + 1], OR_OP) == 0))
        return print_error(get_error_msg(ERR_NO_NAME_REDIRECT), NULL, 84);
    return 0;
}

/**
 * @brief Validates single ampersand syntax
 *
 * @param tokens Array of tokens to validate
 * @param i Current position in tokens array
 * @return int 0 if valid, error code otherwise
 */
static int validate_single_ampersand(char **tokens, int i)
{
    if (tokens[i][0] == AND && strlen(tokens[i]) == 1)
        return print_error(get_error_msg(ERR_INVALID_AND_OPERATOR), NULL, 84);
    return 0;
}

/**
 * @brief Validates if the last token is valid
 *
 * @param tokens Array of tokens to validate
 * @param last_index Index of the last token
 * @return int 0 if valid, error code otherwise
 */
static int validate_last_token(char **tokens, int last_index)
{
    if (last_index > 0 && (tokens[last_index][0] == PIPE ||
    strcmp(tokens[last_index], AND_OP) == 0 ||
    strcmp(tokens[last_index], OR_OP) == 0))
        return print_error(get_error_msg(ERR_INVALID_NULL_COMMAND), NULL, 84);
    return 0;
}

/**
 * @brief Validates the syntax of a token array
 *
 * @param tokens Array of tokens to validate
 * @return int 0 if syntax is valid, error code otherwise
 */
int validate_syntax(char **tokens)
{
    int i = 0;

    if (validate_first_token(tokens) == 84)
        return 84;
    if (validate_parentheses_balance(tokens) == 84)
        return 84;
    while (tokens[i]) {
        if (validate_consecutive_pipes(tokens, i) == 84)
            return 84;
        if (validate_logical_operators(tokens, i) == 84)
            return 84;
        if (validate_redirections(tokens, i) == 84)
            return 84;
        if (validate_single_ampersand(tokens, i) == 84)
            return 84;
        i++;
    }
    return validate_last_token(tokens, i - 1);
}
