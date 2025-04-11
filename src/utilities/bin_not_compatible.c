/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** bin_not_compatible.c
*/

#include "mysh.h"

/**
 * @brief Handles an error when a binary is not compatible.
 *
 * @param arg : The argument (binary name or path) that caused the error.
 */
void bin_not_compatible(char *arg)
{
    if (errno == ENOEXEC) {
        write(2, arg, strlen(arg));
        write(2, ": ", 2);
        write(2, get_error_msg(ERR_WRG_ARCH),
        strlen(get_error_msg(ERR_WRG_ARCH)));
        exit(errno);
    }
}
