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
