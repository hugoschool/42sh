/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** char_utilities.c
*/

#include "mysh.h"

/**
 * @brief Trims leading whitespace from a string.
 *
 * @param line : The string to trim.
 * @return : Pointer to the first non-whitespace character, or NULL if empty.
 */
char *trim_whitespace(char *line)
{
    if (!line)
        return NULL;
    while (*line && (*line == ' ' || *line == '\t'))
        line++;
    return (*line) ? line : NULL;
}

/**
 * @brief Checks if a character is a special token.
 *
 * @param c : The character to check.
 * @return : 1 if it's a special token (; | < >), 0 otherwise.
 */
int is_special_token(char c)
{
    return (c == ';' || c == PIPE || c == REDIR_IN || c == REDIR_OUT);
}

/**
 * @brief Removes quotes from a string in-place.
 *
 * @param str : The string to process.
 */
void strip_quotes(char *str)
{
    int len = (str) ? strlen(str) : 0;
    int i;
    int j = 0;
    int in_quotes = 0;
    char quote_char = 0;

    if (!str)
        return;
    for (i = 0; i < len; i++) {
        if ((str[i] == '\'' || str[i] == '"') &&
        (!in_quotes || quote_char == str[i])) {
            in_quotes = !in_quotes;
            quote_char = (in_quotes) ? str[i] : 0;
            continue;
        }
        str[j] = str[i];
        j++;
    }
    str[j] = '\0';
}

/**
 * @brief Skips whitespace characters during tokenization.
 *
 * @param tl : Token line structure.
 * @param i : Pointer to current position.
 * @param state : Current tokenization state.
 */
void skip_whitespace(token_line_t *tl, int *i, token_state_t *state)
{
    while (tl->line[*i] && !state->in_quotes &&
    (tl->line[*i] == ' ' || tl->line[*i] == '\t'))
        (*i)++;
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
