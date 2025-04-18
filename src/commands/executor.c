/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** executor.c
*/

#include "mysh.h"

/**
 * @brief Executes a command by parsing and processing the command line.
 *
 * @param line : The command line to execute.
 * @return : The status of the command execution.
 */
int main_execute_command(char *line)
{
    ast_node_t *root = parse_line(line);
    int status;

    if (!root)
        return 1;
    status = execute_ast(root);
    free_ast(root);
    return status;
}
