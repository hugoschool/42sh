/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** is.c
*/

#include "mysh.h"

int is_parenthesis(char c)
{
    return (c == OPEN_PAREN || c == CLOSE_PAREN);
}

/**
 * @brief Checks if a character is a special token.
 *
 * @param c : The character to check.
 * @return : 1 if it's a special token (; | < >), 0 otherwise.
 */
int is_special_token(char c)
{
    return (c == ';' || c == PIPE || c == REDIR_IN || c == REDIR_OUT ||
    c == AND || c == OPEN_PAREN || c == CLOSE_PAREN);
}

/**
 * @brief Checks if the current characters form a logical operator.
 *
 * @param line : The command line being processed.
 * @param i : Current position in the line.
 * @return : 1 if a logical operator is found (&&, ||), 0 otherwise.
 */
int is_logical_op(const char *line, int i)
{
    if (!line || !line[i] || !line[i + 1])
        return 0;
    return ((line[i] == '&' && line[i + 1] == '&') ||
    (line[i] == '|' && line[i + 1] == '|'));
}

/**
 * @brief Checks if a node type is a valid redirection type.
 *
 * @param type : The node type to check.
 * @return : 1 if it's a valid redirection type, 0 otherwise.
 */
int is_valid_redirection_type(node_type_t type)
{
    return (type == NODE_REDIR_IN || type == NODE_REDIR_OUT ||
    type == NODE_REDIR_APPEND || type == NODE_REDIR_HEREDOC);
}
