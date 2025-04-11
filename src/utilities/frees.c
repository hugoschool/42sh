/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** frees.c
*/

#include "mysh.h"

/**
 * @brief Frees a simple command structure.
 *
 * @param cmd : The simple command to free.
 */
static void free_simple_command(simple_command_t *cmd)
{
    if (!cmd)
        return;
    if (cmd->in.file)
        free(cmd->in.file);
    if (cmd->out.file)
        free(cmd->out.file);
    free(cmd);
}

/**
 * @brief Frees a complex command structure.
 *
 * @param cmd : The complex command to free.
 */
void free_command(command_t *cmd)
{
    if (!cmd)
        return;
    for (int i = 0; i < cmd->cmd_count; i++)
        free_simple_command(cmd->commands[i]);
    free(cmd->commands);
    free(cmd);
}

/**
 * @brief Frees an array of argument strings.
 *
 * @param args : The argument array to free.
 */
void free_args(char **args)
{
    if (!args)
        return;
    for (int i = 0; args[i]; i++)
        free(args[i]);
    free(args);
}

/**
 * @brief Recursively frees an AST structure.
 *
 * @param node : The root node of the AST to free.
 * @return : Always NULL to facilitate cleanup logic.
 */
char *free_ast(ast_node_t *node)
{
    redir_t *current;
    redir_t *next;

    if (!node)
        return NULL;
    free_ast(node->left);
    free_ast(node->right);
    if (node->args)
        free_args(node->args);
    current = node->redirections;
    while (current) {
        next = current->next;
        free((current->file) ? current->file : NULL);
        free(current);
        current = next;
    }
    free(node);
    return NULL;
}

/**
 * @brief Frees an array of token strings.
 *
 * @param tokens : The token array to free.
 * @param count : Number of tokens to free.
 */
void free_tokens(char **tokens, int count)
{
    for (int i = 0; i < count; i++)
        free(tokens[i]);
}

/**
 * @brief Frees token array on error and returns NULL.
 *
 * @param tokens : The token array to free.
 * @param token_count : Number of tokens to free.
 * @return : NULL to indicate error.
 */
char **free_tokens_on_error(char **tokens, int token_count)
{
    for (int j = 0; j < token_count; j++)
        free(tokens[j]);
    free(tokens);
    return NULL;
}
