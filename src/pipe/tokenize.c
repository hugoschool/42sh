/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** tokenize.c
*/

#include "mysh.h"

/**
 * @brief Handles error during tokenization by freeing resources.
 *
 * @param tokens : The tokens array to free.
 * @param count : Number of tokens to free.
 * @param message : Error message to display.
 * @return : -1 to indicate error.
 */
static int handle_error(char **tokens, int count, const char *message)
{
    perror(message);
    free_tokens(tokens, count);
    return -1;
}

/**
 * @brief Adds an operator token to the tokens array.
 *
 * @param tokens : The tokens array.
 * @param count : Current token count.
 * @param type : The operator character.
 * @param dbl : Flag indicating if it's a double character operator
 * (>>, <<, &&, ||).
 * @return : 0 on success, -1 on failure.
 */
static int add_operator(char **tokens, int count, char type, int dbl)
{
    char *token = malloc((dbl) ? 3 : 2);

    if (!token)
        return -1;
    token[0] = type;
    token[1] = (dbl) ? type : '\0';
    if (dbl)
        token[2] = '\0';
    tokens[count] = token;
    return 0;
}

/**
 * @brief Saves the current token to the tokens array.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @param i : Current position in the line.
 * @return : 0 on success, -1 on failure.
 */
int save_current_token(token_line_t *tl, token_state_t *state, int i)
{
    tl->tokens[state->count] = strndup(&tl->line[state->start],
        i - state->start);
    if (!tl->tokens[state->count])
        return -1;
    state->count++;
    return 0;
}

/**
 * @brief Handles special tokens like |, >, <.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @param i : Current position in the line.
 * @return : 0 or 1 on success (1 for double operators), -1 on failure.
 */
static int handle_special_token(token_line_t *tl, token_state_t *state, int i)
{
    int ret = 0;
    int save = 0;

    if (is_logical_op(tl->line, i))
        return handle_logical_operator(tl, state, i);
    if (state->in_token) {
        ret = save_current_token(tl, state, i);
        if (ret == -1)
            return -1;
        state->in_token = 0;
    }
    save = (tl->line[i] == REDIR_OUT || tl->line[i] == REDIR_IN) &&
    tl->line[i + 1] == tl->line[i];
    if (add_operator(tl->tokens, state->count, tl->line[i], save) == -1)
        return -1;
    state->count++;
    return (save) ? 1 : 0;
}

/**
 * @brief Handles whitespace characters during tokenization.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @param i : Current position in the line.
 * @return : 0 on success, -1 on failure.
 */
static int handle_whitespace(token_line_t *tl, token_state_t *state, int i)
{
    int ret;

    if (state->in_token) {
        ret = save_current_token(tl, state, i);
        if (ret == -1)
            return -1;
        state->in_token = 0;
    }
    return 0;
}

/**
 * @brief Handles quote characters during tokenization.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @param i : Current position in the line.
 * @return : 0 on success.
 */
int handle_quotes_token(token_line_t *tl, token_state_t *state, int i)
{
    int save = 0;

    if (tl->line[i] == DBL_QUOTE || tl->line[i] == QUOTE) {
        if (!state->in_quotes) {
            state->in_quotes = 1;
            state->quote_char = tl->line[i];
            save = 1;
        }
        if (save == 0 && tl->line[i] == state->quote_char)
            state->in_quotes = 0;
        if (!state->in_token) {
            state->start = i;
            state->in_token = 1;
        }
    }
    return 0;
}

/**
 * @brief Adds the last token to the tokens array and finalizes it.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @return : Token count on success, -1 on failure.
 */
static int add_last_token(token_line_t *tl, token_state_t *state)
{
    if (state->in_token) {
        tl->tokens[state->count] = strdup(&tl->line[state->start]);
        if (!tl->tokens[state->count])
            return -1;
        state->count++;
    }
    tl->tokens[state->count] = NULL;
    return state->count;
}

/**
 * @brief Processes a single character during tokenization.
 *
 * @param tl : Token line structure.
 * @param state : Current tokenization state.
 * @param i : Current position in the line.
 * @return : 0 for normal processing, 1 for whitespace, 2 for double operators,
 * -2 for errors.
 */
static int handle_special_cases(token_line_t *tl, token_state_t *state, int i)
{
    int ret;

    if (tl->line[i] == DBL_QUOTE || tl->line[i] == QUOTE)
        return handle_quotes_token(tl, state, i);
    if (!(state->in_quotes) && (tl->line[i] == ' ' || tl->line[i] == '\t')) {
        ret = handle_whitespace(tl, state, i);
        return (ret == -1) ? -2 : 1;
    }
    if (!(state->in_quotes) && is_logical_op(tl->line, i)) {
        ret = handle_logical_operator(tl, state, i);
        return (ret == -1) ? -2 : 2;
    }
    if (!(state->in_quotes) && is_special_token(tl->line[i])) {
        ret = handle_special_token(tl, state, i);
        if (ret == -1)
            return -2;
        return ret == 1 ? 2 : 1;
    }
    return 0;
}

static int process_token_char(token_line_t *tl, token_state_t *state, int i)
{
    int special_result = handle_special_cases(tl, state, i);

    if (special_result != 0)
        return special_result;
    if (!(state->in_token)) {
        state->start = i;
        state->in_token = 1;
    }
    return 0;
}

/**
 * @brief Tokenizes a command line into an array of strings.
 *
 * @param line : The command line to tokenize.
 * @param tokens : Pre-allocated array to store the tokens.
 * @return : Number of tokens on success, negative value on error.
 */
int tokenize(char *line, char **tokens)
{
    token_line_t tl = {line, tokens, 0};
    token_state_t state = {0, 0, 0, 0, 0};
    int ret;

    for (int i = 0; tl.line[i]; i++) {
        ret = process_token_char(&tl, &state, i);
        if (ret == -2)
            return handle_error(tokens, state.count,
            "Memory allocation failed");
        if (ret == 2)
            i++;
    }
    ret = add_last_token(&tl, &state);
    if (ret == -1)
        return handle_error(tokens, state.count, "strdup failed");
    return ret;
}
