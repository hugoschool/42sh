/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** command_struct.c
*/

#include "mysh.h"

/**
 * @brief Handles an input redirection for a simple command.
 *
 * @param cmd : The simple command structure to update.
 * @param type : The type of redirection to set.
 * @param file_copy : The file path for redirection.
 * @return : 0 on success.
 */
static int handle_input_redirection(simple_command_t *cmd,
    redirection_type_t type, char *file_copy)
{
    free((cmd->in.file) ? cmd->in.file : NULL);
    cmd->in.type = type;
    cmd->in.file = file_copy;
    return 0;
}

/**
 * @brief Handles an output redirection for a simple command.
 *
 * @param cmd : The simple command structure to update.
 * @param type : The type of redirection to set.
 * @param file_copy : The file path for redirection.
 * @return : 0 on success.
 */
static int handle_output_redirection(simple_command_t *cmd,
    redirection_type_t type, char *file_copy)
{
    free((cmd->out.file) ? cmd->out.file : NULL);
    cmd->out.type = type;
    cmd->out.file = file_copy;
    return 0;
}

/**
 * @brief Checks for conflicting redirection types and reports errors.
 *
 * @param cmd : The simple command to check for conflicts.
 * @param in_type : Flag indicating if this is an input redirection.
 * @param out_type : Flag indicating if this is an output redirection.
 * @param file_copy : The file path being used (freed on error).
 * @return : 0 if no conflicts, 84 if conflicts are found.
 */
static int check_redirection_conflicts(simple_command_t *cmd, int in_type,
    int out_type, char *file_copy)
{
    if (in_type && cmd->in.type != REDIR_NONE) {
        free(file_copy);
        return print_error(get_error_msg(ERR_AMB_IN_REDIR), NULL, 84);
    }
    if (out_type && cmd->out.type != REDIR_NONE) {
        free(file_copy);
        return print_error(get_error_msg(ERR_AMB_OUT_REDIR), NULL, 84);
    }
    return 0;
}

/**
 * @brief Sets a redirection for a command.
 *
 * @param cmd : The simple command to set redirection for.
 * @param type : The type of redirection (in, out, append, heredoc).
 * @param file : The file path or content for the redirection.
 * @return : 0 on success, 84 on failure.
 */
int set_redirection(simple_command_t *cmd, redirection_type_t type, char *file)
{
    char *file_copy = (file) ? strdup(file) : NULL;
    int in_type = (type == REDIR_IN_SIMPLE || type == REDIR_IN_HEREDOC);
    int out_type = (type == REDIR_OUT_SIMPLE || type == REDIR_OUT_APPEND);
    int status = 0;

    if (!cmd || !file || !file_copy)
        return 84;
    status = check_redirection_conflicts(cmd, in_type, out_type, file_copy);
    if (status == 84)
        return 84;
    if (in_type)
        return handle_input_redirection(cmd, type, file_copy);
    if (out_type)
        return handle_output_redirection(cmd, type, file_copy);
    free(file_copy);
    return 84;
}
