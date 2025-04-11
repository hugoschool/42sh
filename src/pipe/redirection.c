/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** pipe_redirection.c
*/

#include "mysh.h"

/**
 * @brief Checks if a token is a redirection operator and identifies its type.
 *
 * @param token : The token to check.
 * @param type : Pointer to store the redirection type if found.
 * @return : 1 if token is a redirection, 0 otherwise.
 */
static int is_redirection(char *token, redirection_type_t *type)
{
    if (!token || !token[0])
        return 0;
    if (token[0] == REDIR_OUT) {
        if (token[1] == REDIR_OUT) {
            *type = REDIR_OUT_APPEND;
            return 1;
        }
        *type = REDIR_OUT_SIMPLE;
        return 1;
    }
    if (token[0] == REDIR_IN) {
        if (token[1] == REDIR_IN) {
            *type = REDIR_IN_HEREDOC;
            return 1;
        }
        *type = REDIR_IN_SIMPLE;
        return 1;
    }
    return 0;
}

/**
 * @brief Handles a redirection token and updates the command structure.
 *
 * @param tokens : Array of all tokens.
 * @param cmd : The simple command to update with redirection.
 * @param i : Pointer to current position in tokens array.
 * @param args : Current command arguments (freed on error).
 * @return : 1 if handled redirection, 0 if not a redirection, 84 on error.
 */
static int handle_redirection(char **tokens, simple_command_t *cmd,
    int *i, char **args)
{
    redirection_type_t type;

    if (is_redirection(tokens[*i], &type)) {
        if (!tokens[*i + 1] || is_special_token(tokens[*i + 1][0])) {
            free(args);
            return print_error(get_error_msg(ERR_NO_NAME_REDIRECT),
            NULL, 84);
        }
        if (set_redirection(cmd, type, tokens[*i + 1]) == 84) {
            free(args);
            return 84;
        }
        *i += 2;
        return 1;
    }
    return 0;
}

/**
 * @brief Parses and processes all redirections for a command.
 * Builds a new argument array without redirection tokens.
 *
 * @param tokens : Array of command tokens including redirections.
 * @param cmd : The simple command to update.
 * @return : 0 on success, 84 on error.
 */
int parse_redirections(char **tokens, simple_command_t *cmd)
{
    int i = 0;
    int j = 0;
    char **args = malloc(sizeof(char *) * (cmd->arg_count + 1));

    if (!args)
        return 84;
    while (tokens[i]) {
        if (handle_redirection(tokens, cmd, &i, args) && i < 0)
            return 84;
        args[j] = tokens[i];
        j++;
        i++;
    }
    args[j] = NULL;
    free(cmd->args);
    cmd->args = args;
    cmd->arg_count = j;
    return 0;
}
