/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** need_multiline.c
*/

#include "mysh.h"

/**
 * Processes a character to count the different types of parentheses
 *
 * @param c Character to process
 * @param paren_count Pointer to the parentheses counter
 * @param bracket_count Pointer to the brackets counter
 * @param brace_count Pointer to the braces counter
 */
static void process_open_bracket(char c, int *paren_count, int *bracket_count,
    int *brace_count)
{
    switch (c) {
        case OPEN_PAREN:
            (*paren_count)++;
            break;
        case OPEN_BRACKET:
            (*bracket_count)++;
            break;
        case OPEN_BRACE:
            (*brace_count)++;
            break;
        default:
            break;
    }
}

static void process_bracket(char c, int *paren_count, int *bracket_count,
    int *brace_count)
{
    switch (c) {
        case CLOSE_PAREN:
            (*paren_count)--;
            break;
        case CLOSE_BRACKET:
            (*bracket_count)--;
            break;
        case CLOSE_BRACE:
            (*brace_count)--;
            break;
        default:
            process_open_bracket(c, paren_count, bracket_count, brace_count);
    }
}

/**
 * Checks whether a line contains unclosed parentheses, brackets or braces
 *
 * @param line Line to be checked
 * @param bracket_type Type of unclosed parenthesis (if found)
 * @return 1 if parentheses are unclosed, 0 otherwise
 */
static int set_bracket(int paren_count, int bracket_count, int brace_count,
    char *bracket_type)
{
    int ret_value = (paren_count > 0 || bracket_count > 0 || brace_count > 0);

    if (paren_count > 0 && bracket_type) {
        *bracket_type = OPEN_PAREN;
        return ret_value;
    }
    if (bracket_count > 0 && bracket_type) {
        *bracket_type = OPEN_BRACKET;
        return ret_value;
    }
    if (brace_count > 0 && bracket_type) {
        *bracket_type = OPEN_BRACE;
        return ret_value;
    }
    return ret_value;
}

int has_unclosed_brackets(const char *line, char *bracket_type)
{
    int paren_count = 0;
    int bracket_count = 0;
    int brace_count = 0;
    int in_quotes = 0;
    char quote_char = 0;

    if (!line)
        return 0;
    for (int i = 0; line[i]; i++) {
        if ((line[i] == QUOTE || line[i] == DBL_QUOTE) &&
        (!in_quotes || line[i] == quote_char)) {
            quote_char = (in_quotes) ? 0 : line[i];
            in_quotes = (in_quotes) ? 0 : 1;
            continue;
        }
        if (in_quotes)
            continue;
        process_bracket(line[i], &paren_count, &bracket_count, &brace_count);
    }
    return set_bracket(paren_count, bracket_count, brace_count, bracket_type);
}

/**
 * @brief Checks if a line ends with a backslash or an operator
 *
 * @param line The line to check
 * @param is_operator Pointer to store if continuation is due to operator (1)
 * or backslash (0)
 * @return 1 if the line ends with a continuation character, 0 otherwise
 */
int has_trailing_continuation(const char *line, int *is_operator)
{
    int len = (line) ? strlen(line) : 0;

    if (!line || len == 0)
        return 0;
    if (line[len - 1] == BACKSLASH) {
        if (is_operator)
            *is_operator = 0;
        return 1;
    }
    if ((line[len - 1] == REDIR_IN || line[len - 1] == REDIR_OUT ||
    line[len - 1] == PIPE || line[len - 1] == ';') ||
    (len >= 2 && (line[len - 2] == REDIR_IN && line[len - 1] == REDIR_IN) ||
    (line[len - 2] == REDIR_OUT && line[len - 1] == REDIR_OUT))) {
        if (is_operator)
            *is_operator = 1;
        return 1;
    }
    return 0;
}

/**
 * @brief Checks if a line contains unclosed quotes
 *
 * @param line The line to check
 * @param quote_type Pointer to store the type of unclosed quote (', " or 0)
 * @return 1 if quotes are unclosed, 0 otherwise
 */
int has_unclosed_quotes(const char *line, char *quote_type)
{
    char current_quote = 0;
    int in_quotes = 0;

    if (!line)
        return 0;
    for (int i = 0; line[i]; i++) {
        if ((line[i] == QUOTE || line[i] == DBL_QUOTE) &&
        (!in_quotes || line[i] == current_quote)) {
            current_quote = (in_quotes) ? 0 : line[i];
            in_quotes = (in_quotes) ? 0 : 1;
        }
    }
    if (in_quotes && quote_type)
        *quote_type = current_quote;
    return in_quotes;
}
