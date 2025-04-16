/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** multiline.c
*/

#include "mysh.h"

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

/**
 * @brief Reads input in multiline mode when quotes are unclosed
 *
 * @param initial_line The initial line with unclosed quotes
 * @param quote_type The type of unclosed quote
 * @return A complete new line or NULL on error
 */
static ssize_t read_input_line(char **line)
{
    ssize_t read;
    size_t len = 0;

    if (isatty(STDIN_FILENO))
        printf("%s", MULTI_PROMPT);
    read = getline(line, &len, stdin);
    if (read > 0 && (*line)[read - 1] == '\n') {
        (*line)[read - 1] = '\0';
        read--;
    }
    return read;
}

static char *append_line_to_buffer(char *buffer, char *line,
    char *initial_line, ssize_t read)
{
    char *new_buffer;

    new_buffer = malloc(strlen(buffer) + read + 2);
    if (!new_buffer) {
        free(line);
        return buffer;
    }
    strcpy(new_buffer, buffer);
    strcat(new_buffer, line);
    if (buffer != initial_line)
        free(buffer);
    return new_buffer;
}

char *read_multiline_quotes(char *initial_line, char quote_type)
{
    char *buffer = initial_line;
    char *line = NULL;
    ssize_t read;
    char current_quote = quote_type;

    if (!isatty(STDIN_FILENO))
        return buffer;
    while (1) {
        read = read_input_line(&line);
        if (read == -1) {
            free((line) ? line : NULL);
            return buffer;
        }
        buffer = append_line_to_buffer(buffer, line, initial_line, read);
        if (!has_unclosed_quotes(buffer, &current_quote)) {
            free(line);
            return buffer;
        }
    }
    return buffer;
}
