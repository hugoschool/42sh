/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** error_handling.c
*/

#include "mysh.h"

const char *error_messages[] = {
    ": Permission denied.\n",
    ": Command not found.\n",
    ": No such file or directory.\n",
    ": Not a directory.\n",
    "Exec format error. Binary file not executable.\n",
    ": Cannot create file.\n",
    "cd: too many arguments\n",
    "setenv: Too many arguments.\n",
    "unsetenv: Too few arguments.\n",
    "unsetenv: Wrong number of arguments.\n",
    "unsetenv: incorrect argument.\n",
    "Ambiguous input redirect.\n",
    "Ambiguous output redirect.\n",
    "Missing name for redirect.\n",
    "Missing name for redirection\n"
    "Invalid null command.\n",
    "exit: Expression Syntax.\n",
    "exit: Badly formed number.\n",
    ": Failed to create heredoc.\n"
    "Unknown node type\n",
    "setenv: Variable name must begin with a letter.\n",
    "setenv: Variable name must contain alphanumeric characters.\n",
    "unsetenv: Variable name must contain alphanumeric characters.\n"
};

/**
 * @brief Gets the error message for a specific error type.
 *
 * @param type : The error type.
 * @return : The corresponding error message.
 */
char *get_error_msg(error_type_t type)
{
    return (char *)error_messages[type];
}

/**
 * @brief Prints an error message to stderr.
 *
 * @param cmd : The command that caused the error (can be NULL).
 * @param msg : The error message (can be NULL).
 * @param nb : The return value to pass through.
 * @return : The nb parameter value.
 */
int print_error(char *cmd, char *msg, int nb)
{
    if (cmd)
        write(2, cmd, strlen(cmd));
    if (msg)
        write(2, msg, strlen(msg));
    return nb;
}
