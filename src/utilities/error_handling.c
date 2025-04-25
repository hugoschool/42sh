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
    ": Exec format error. Binary file not executable.\n",
    ": Cannot create file.\n",
    ": Too many arguments.\n",
    ": Too few arguments.\n",
    "unsetenv: Wrong number of arguments.\n",
    "unsetenv: incorrect argument.\n",
    "Ambiguous input redirect.\n",
    "Ambiguous output redirect.\n",
    "Missing name for redirect.\n",
    "Missing name for redirection\n",
    "Invalid null command.\n",
    "exit: Expression Syntax.\n",
    ": Badly formed number.\n",
    ": Failed to create heredoc.\n",
    "Unknown node type\n",
    "setenv: Variable name must begin with a letter.\n",
    "setenv: Variable name must contain alphanumeric characters.\n",
    "unsetenv: Variable name must contain alphanumeric characters.\n",
    "Invalid operator '&'. Did you mean '&&'?\n",
    "Invalid operator '|'. Did you mean '||'?\n",
    "Invalid null command after '&&'.\n",
    "Invalid null command after '||'.\n",
    "Too many ('s.\n",
    "Too many )'s.\n",
    ": Event not found.\n",
    ": No match.\n"
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

/**
 * @brief Prints an error message depending on errno value.
 *
 * @param cmd : Command name.
 * @return : Exit code 1.
 */
int get_errno_error(char *cmd)
{
    if (errno == ENOEXEC)
        return print_error(cmd, get_error_msg(ERR_WRG_ARCH), 1);
    if (errno != ENOEXEC)
        return print_error(cmd, get_error_msg(ERR_NOT_FOUND), 1);
    return 1;
}

char *my_strsignal(int signal)
{
    switch (signal) {
        case 8:
            return "Floating exception";
        default:
            return strsignal(signal);
    }
}
