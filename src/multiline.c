/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** multiline.c
*/

#include "mysh.h"

/**
 * @brief Reads input line in multiline mode
 *
 * @param line Pointer to store the read line
 * @return Number of bytes read or -1 on error
 */
static ssize_t read_input_line(char **line)
{
    ssize_t read;
    char *input;
    size_t len = 0;

    if (isatty(STDIN_FILENO)) {
        input = readline(MULTI_PROMPT);
        if (!input)
            return -1;
        *line = input;
        read = strlen(input);
    } else {
        read = getline(line, &len, stdin);
        if (read > 0 && (*line)[read - 1] == '\n') {
            (*line)[read - 1] = '\0';
            read--;
        }
    }
    return read;
}

/**
 * @brief Appends a new line to an existing buffer
 *
 * @param buffer Current content buffer
 * @param line Line to append
 * @param initial_line The original line
 * (to avoid freeing it if buffer == initial_line)
 * @param read Number of bytes read in line
 * @return Updated buffer with appended line
 */
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
    free(line);
    return new_buffer;
}

/**
 * @brief Helper function to check if multiline input is complete
 *
 * @param buffer Current input buffer
 * @param check_type Type of check to perform: 0 for quotes, 1 for continuation
 * 2 for unclosed brackets
 * @param param Pointer to store quote type (for quotes) or is_operator flag
 * (for continuation)
 * @return 1 if more input is needed, 0 if input is complete
 */
static int needs_more_input(char *buffer, int check_type, void *param)
{
    switch (check_type) {
        case 0:
            return has_unclosed_quotes(buffer, (char *)param);
        case 1:
            return has_trailing_continuation(buffer, (int *)param);
        case 2:
            return has_unclosed_brackets(buffer, (char *)param);
        default:
            return 0;
    }
    return 0;
}

/**
 * @brief Handles multiline input processing for both quote and continuation
 * cases
 *
 * @param initial_line The initial line with unclosed quotes or continuation
 * @param check_type Type of check to perform: 0 for quotes, 1 for continuation
 * @param param Quote type (for quotes) or is_operator flag (for continuation)
 * @return Complete input buffer
 */
static char *handle_continuation(char *buffer, int check_type, void *param)
{
    int buffer_len = 0;

    if (check_type == 1 && !(*(int *)param)) {
        buffer_len = strlen(buffer);
        if (buffer_len > 0 && buffer[buffer_len - 1] == BACKSLASH)
            buffer[buffer_len - 1] = '\0';
    }
    return buffer;
}

static char *continue_reading_input(char *buffer, int check_type, void *param,
    char *initial_line)
{
    char *line = NULL;
    ssize_t read;
    int need_more;

    while (1) {
        buffer = handle_continuation(buffer, check_type, param);
        read = read_input_line(&line);
        if (read == -1)
            return buffer;
        buffer = append_line_to_buffer(buffer, line, initial_line, read);
        need_more = needs_more_input(buffer, check_type, param);
        if (!need_more)
            break;
    }
    return buffer;
}

char *read_multiline_input(char *initial_line, int check_type, void *param)
{
    int need_more;
    char *buffer;

    if (!isatty(STDIN_FILENO))
        return initial_line;
    buffer = initial_line;
    buffer = handle_continuation(buffer, check_type, param);
    need_more = needs_more_input(buffer, check_type, param);
    if (!need_more)
        return buffer;
    return continue_reading_input(buffer, check_type, param, initial_line);
}
