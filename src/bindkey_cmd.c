/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** bindkey_cmd.c
*/

#include "mysh.h"
#include <readline/readline.h>
#include <readline/history.h>

/**
 * @brief Displays all defined bindkeys
 */
static void display_bindkeys(void)
{
    bindkey_mapping_t *current = get_bindkey_mappings(NULL);

    if (!current) {
        printf("No bindkey defined\n");
        return;
    }
    printf("Defined key bindings:\n");
    while (current) {
        printf("  %-15s %s\n", current->key_seq, current->func_name);
        current = current->next;
    }
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
    add_bindkey_mapping(key_seq, func_name);
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
    if (count == 1 && (strcmp(args[1], "--help") == 0 ||
    strcmp(args[1], "-h") == 0)) {
        display_bindkey_help();
        return 0;
    }
    if (count == 0) {
        display_bindkeys();
        return 0;
    }
    if (count == 2)
        return handle_bindkey(args[1], args[2]);
    printf("Usage: bindkey [key_sequence function_name]\n");
    printf("       bindkey --help for more information\n");
    return 1;
}
