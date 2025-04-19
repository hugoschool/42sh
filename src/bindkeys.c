/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** bindkeys.c
*/

#include "mysh.h"
#include <readline/readline.h>
#include <readline/history.h>

static const bindkey_func_t bindkey_funcs[] = {
    { "beginning-of-line", rl_beg_of_line },
    { "end-of-line", rl_end_of_line },
    { "forward-char", rl_forward_char },
    { "backward-char", rl_backward_char },
    { "forward-word", rl_forward_word },
    { "backward-word", rl_backward_word },
    { "clear-screen", rl_clear_screen },
    { "kill-line", rl_kill_line },
    { "kill-word", rl_kill_word },
    { "backward-kill-word", rl_backward_kill_word },
    { "complete", rl_complete },
    { "possible-completions", rl_possible_completions },
    { "insert-completions", rl_insert_completions },
    { "previous-history", rl_get_previous_history },
    { "next-history", rl_get_next_history },
    { "history-search-backward", rl_history_search_backward },
    { "history-search-forward", rl_history_search_forward },
    { "transpose-chars", rl_transpose_chars },
    { "transpose-words", rl_transpose_words },
    { "upcase-word", rl_upcase_word },
    { "downcase-word", rl_downcase_word },
    { "capitalize-word", rl_capitalize_word },
    { NULL, NULL }
};

/**
 * @brief Finds a readline function by its name
 *
 * @param func_name: Name of the readline function
 * @return: Pointer to the function or NULL if not found
 */
static rl_command_func_t *find_readline_function(const char *func_name)
{
    for (int i = 0; bindkey_funcs[i].name; i++) {
        if (strcmp(bindkey_funcs[i].name, func_name) == 0)
            return bindkey_funcs[i].func;
    }
    return NULL;
}

/**
 * @brief Converts an escape sequence into a character
 *
 * @param seq: Sequence to convert (e.g., "\C-a" for Ctrl+A)
 * @return: Keycode or -1 if invalid
 */
static int parse_control_sequence(const char *seq)
{
    if (seq[3] >= 'a' && seq[3] <= 'z')
        return seq[3] - 'a' + 1;
    if (seq[3] >= 'A' && seq[3] <= 'Z')
        return seq[3] - 'A' + 1;
    return seq[3];
}

static int parse_escaped_char(const char *seq)
{
    switch (seq[1]) {
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'b':
            return '\b';
        default:
            return seq[1];
    }
}

static int parse_backslash_sequence(const char *seq)
{
    if (strlen(seq) < 2)
        return -1;
    if (seq[1] == 'C' && seq[2] == '-' && strlen(seq) >= 4)
        return parse_control_sequence(seq);
    if (seq[1] == 'e' || (seq[1] == 'M' && seq[2] == '-' && strlen(seq) >= 4))
        return 0x1B;
    return parse_escaped_char(seq);
}

static int parse_key_sequence(const char *seq)
{
    if (!seq || !*seq)
        return -1;
    if (strlen(seq) == 1)
        return (unsigned char)seq[0];
    if (seq[0] == BACKSLASH)
        return parse_backslash_sequence(seq);
    return -1;
}

/**
 * @brief Processes a bindkey command and configures readline
 *
 * @param key_seq: Key sequence to bind
 * @param func_name: Name of the readline function to execute
 * @return: 0 on success, 1 otherwise
 */
int handle_bindkey(const char *key_seq, const char *func_name)
{
    int key = parse_key_sequence(key_seq);
    rl_command_func_t *func = find_readline_function(func_name);

    if (key == -1 || !func) {
        fprintf(stderr, "bindkey: Invalid key sequence or function\n");
        return 1;
    }
    rl_bind_key(key, func);
    return 0;
}

/**
 * @brief Initializes default keyboard shortcuts
 */
void init_default_bindkeys(void)
{
    rl_bind_key(1, rl_beg_of_line);
    rl_bind_key(5, rl_end_of_line);
    rl_bind_key(12, rl_clear_screen);
    rl_bind_key(2, rl_backward_char);
    rl_bind_key(6, rl_forward_char);
    rl_bind_key(14, rl_get_next_history);
    rl_bind_key(16, rl_get_previous_history);
}

/**
 * @brief Processes the bindkey builtin command
 *
 * @param args: Command arguments
 * @param count: Number of arguments
 * @return: 0 on success, 1 otherwise
 */
int my_bindkey(char *args[], int count)
{
    if (count == 0)
        return 0;
    if (count == 2)
        return handle_bindkey(args[1], args[2]);
    printf("Usage: bindkey [key_sequence function_name]\n");
    return 1;
}
