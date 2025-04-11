/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** tokenize.c
*/

#include "mysh.h"

/**
 * @brief Calculates the length of a token respecting quotes.
 *
 * @param tl : Token line structure containing the line to process.
 * @param state : Current tokenization state.
 * @param i : Pointer to current position in the line.
 * @return : Length of the token.
 */
static int get_token_length(token_line_t *tl, token_state_t *state, int *i)
{
    int start = *i;

    while (tl->line[*i]) {
        handle_quotes_token(tl, state, *i);
        (*i)++;
        if (!(state->in_quotes) && (tl->line[*i] == ' ' ||
        tl->line[*i] == '\t' || !tl->line[*i] ||
        is_special_token(tl->line[*i])))
            break;
    }
    return *i - start;
}

/**
 * @brief Adds a normal (non-special) token to the tokens array.
 *
 * @param tl : Token line structure containing the line and tokens array.
 * @param state : Current tokenization state.
 * @param i : Pointer to current position in the line.
 * @return : 1 on success, 0 on failure.
 */
static int add_normal_token(token_line_t *tl, token_state_t *state, int *i)
{
    int start = *i;
    int len;
    char *token;

    len = get_token_length(tl, state, i);
    token = malloc(len + 1);
    if (!token)
        return 0;
    strncpy(token, &tl->line[start], len);
    token[len] = '\0';
    strip_quotes(token);
    tl->tokens[state->count] = token;
    state->count++;
    return 1;
}

/**
 * @brief Adds a special token (like |, >, <) to the tokens array.
 *
 * @param tl : Token line structure containing the line and tokens array.
 * @param state : Current tokenization state.
 * @param i : Pointer to current position in the line.
 * @return : 1 on success, 0 on failure.
 */
static int add_special_token(token_line_t *tl, token_state_t *state, int *i)
{
    char special = tl->line[*i];
    int double_char = (special == REDIR_OUT || special == REDIR_IN) &&
    tl->line[*i + 1] == special;

    tl->tokens[state->count] = malloc(double_char ? 3 : 2);
    if (!tl->tokens[state->count])
        return 0;
    tl->tokens[state->count][0] = special;
    if (double_char) {
        tl->tokens[state->count][1] = special;
        tl->tokens[state->count][2] = '\0';
        (*i) += 2;
    } else {
        tl->tokens[state->count][1] = '\0';
        (*i)++;
    }
    state->count++;
    return 1;
}

/**
 * @brief Resizes the tokens array when it's full.
 *
 * @param tl : Token line structure to resize.
 * @return : 1 on success, 0 on failure.
 */
static int resize_tokens_array(token_line_t *tl)
{
    int new_size = tl->tokens_max * 2;
    char **new_tokens = realloc(tl->tokens, new_size * sizeof(char *));

    if (!new_tokens)
        return 0;
    tl->tokens = new_tokens;
    tl->tokens_max = new_size;
    return 1;
}

/**
 * @brief Processes a line and tokenizes it with proper handling of quotes.
 *
 * @param line : The input line to tokenize.
 * @param tokens : The array to store tokens.
 * @param tokens_max : Maximum size of the tokens array.
 * @return : The tokens array, or NULL on error.
 */
char **process_line(char *line, char **tokens, int tokens_max)
{
    token_line_t tl = {line, tokens, tokens_max};
    token_state_t state = {0, 0, 0, 0, 0};
    int i = 0;

    while (tl.line[i]) {
        skip_whitespace(&tl, &i, &state);
        if (!tl.line[i])
            break;
        if (!state.in_quotes && is_special_token(tl.line[i]) &&
        !add_special_token(&tl, &state, &i))
            return free_tokens_on_error(tl.tokens, state.count);
        if (!add_normal_token(&tl, &state, &i))
            return free_tokens_on_error(tl.tokens, state.count);
        if (state.count >= tl.tokens_max - 1 && !resize_tokens_array(&tl))
            return NULL;
    }
    tl.tokens[state.count] = NULL;
    return tl.tokens;
}

/**
 * @brief Tokenizes a command line respecting quoted sections.
 *
 * @param line : The command line to tokenize.
 * @return : Array of token strings, or NULL on error.
 */
char **tokenize_with_quotes(char *line)
{
    int tokens_max = 100;
    char **tokens = malloc(sizeof(char *) * tokens_max);

    if (!tokens)
        return NULL;
    return process_line(line, tokens, tokens_max);
}
