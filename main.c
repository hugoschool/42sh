/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** main.c
*/

#include "mysh.h"

/**
 * @brief Processes special commands like help and exit.
 *
 * @param line : The command line to check.
 * @param last_status : The status of the last executed command.
 * @return : 1 if a special command was processed, 0 otherwise.
 */
static int process_special_commands(char *line, int last_status)
{
    if (strcmp(line, "-h") == 0 || strcmp(line, "--help") == 0) {
        print_help();
        return 1;
    }
    if (strcmp(line, EXIT) == 0) {
        free(line);
        exit(last_status);
    }
    return 0;
}

/**
 * @brief Handles EOF (Ctrl+D) condition by exiting the shell.
 *
 * @param line : The line buffer to free (if allocated).
 * @param last_status : The status of the last executed command.
 */
static void handle_eof(char *line, int last_status)
{
    if (isatty(STDIN_FILENO))
        write(1, "exit\n", 5);
    if (line)
        free(line);
    exit(last_status);
}

/**
 * @brief Handles line continuation and unclosed quotes in input
 *
 * @param line The input line to process
 * @param quote_type Pointer to store the type of unclosed quote
 * @return The processed line (possibly extended with additional input)
 */
static char *process_multiline(char *line, int type, void *param)
{
    char *multiline_buffer = read_multiline_input(line, type, param);

    if (multiline_buffer != line) {
        free(line);
        line = multiline_buffer;
    }
    return line;
}

static char *handle_line_continuation(char *line, char *quote_type)
{
    int is_operator = 0;
    char bracket_type = 0;

    if (has_unclosed_quotes(line, quote_type))
        line = process_multiline(line, 0, &quote_type);
    if (has_trailing_continuation(line, &is_operator))
        line = process_multiline(line, 1, &is_operator);
    if (has_unclosed_brackets(line, &bracket_type))
        line = process_multiline(line, 2, &bracket_type);
    return line;
}

int main(void)
{
    char *line = NULL;
    int last_status = 0;
    char quote_type = 0;
    char *prompt = NULL;

    setup_environment();
    setup_signal_handlers();
    setup_bindkeys();
    setup_config_files();
    while (1) {
        line = my_readline(display_prompt());
        if (!line)
            handle_eof(line, last_status);
        line = handle_line_continuation(line, &quote_type);
        save_history(line);
        if (process_special_commands(line, last_status))
            continue;
        last_status = main_execute_command(line);
    }
    return 0;
}
