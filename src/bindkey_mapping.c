/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** bindkey_mapping.c
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
 * @brief Displays available readline functions for bindkey
 */
void display_bindkey_help(void)
{
    printf("Usage: bindkey [key_sequence function_name]\n\n");
    printf("Available functions:\n");
    for (int i = 0; bindkey_funcs[i].name; i++) {
        if (i % 3 == 0 && i > 0)
            printf("\n");
        printf("  %-20s", bindkey_funcs[i].name);
    }
    printf("\n\n");
    printf("Key sequence examples:\n");
    printf("  \"\\C-a\"    Ctrl+A\n");
    printf("  \"\\M-f\"    Meta+F (usually Alt+F)\n");
    printf("  \"\\e\"      Escape key\n");
    printf("  \"a\"       Single character 'a'\n");
}

/**
 * @brief Gets or sets the bindkey mappings
 *
 * @param new_mappings New mappings to set, or NULL to just get
 * @return Current bindkey mappings
 */
bindkey_mapping_t *get_bindkey_mappings(bindkey_mapping_t *new_mappings)
{
    static bindkey_mapping_t *bindkey_mappings = NULL;

    if (new_mappings != NULL)
        bindkey_mappings = new_mappings;
    return bindkey_mappings;
}

/**
 * @brief Finds a readline function by its name
 *
 * @param func_name: Name of the readline function
 * @return: Pointer to the function or NULL if not found
 */
rl_command_func_t *find_readline_function(const char *func_name)
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

/**
 * @brief Parses a key sequence string into a key code
 *
 * @param seq: Key sequence string
 * @return: Key code or -1 if invalid
 */
int parse_key_sequence(const char *seq)
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
 * @brief Adds a new bindkey mapping
 *
 * @param key_seq The key sequence string
 * @param func_name The function name
 */
void add_bindkey_mapping(const char *key_seq, const char *func_name)
{
    bindkey_mapping_t *new_mapping;
    bindkey_mapping_t *current;
    bindkey_mapping_t *mappings = get_bindkey_mappings(NULL);

    current = mappings;
    while (current) {
        if (strcmp(current->key_seq, key_seq) == 0) {
            free(current->func_name);
            current->func_name = strdup(func_name);
            return;
        }
        current = current->next;
    }
    new_mapping = malloc(sizeof(bindkey_mapping_t));
    if (!new_mapping)
        return;
    new_mapping->key_seq = strdup(key_seq);
    new_mapping->func_name = strdup(func_name);
    new_mapping->next = mappings;
    get_bindkey_mappings(new_mapping);
}
