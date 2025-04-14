/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** error_types.h
*/

#ifndef ERROR_TYPES_H
    #define ERROR_TYPES_H

/**
 * @brief Enumération des types d'erreurs possibles
 */
typedef enum {
    ERR_PERMISSION,
    ERR_NOT_FOUND,
    ERR_NO_SUCH_FILE,
    ERR_NOT_A_DIR,
    ERR_WRG_ARCH,
    ERR_CANT_CREATE_FILE,
    ERR_CD_TOO_MANY,
    ERR_SETENV_TOO_MANY,
    ERR_UNSETENV_TOO_FEW,
    ERR_UNSETENV_WRONG_NUM,
    ERR_UNSETENV_WRONG_ARG,
    ERR_AMB_IN_REDIR,
    ERR_AMB_OUT_REDIR,
    ERR_NO_NAME_REDIRECT,
    ERR_NO_NAME_REDIRECTION,
    ERR_INVALID_NULL_COMMAND,
    ERR_EXIT_SYNTAX,
    ERR_EXIT_BAD_FORMAT,

    ERR_HEREDOC_FAILED,
    ERR_INVALID_OPERATOR,
    ERR_SETENV_BEGIN_LETTER,
    ERR_SETENV_ALPHA_CARA,
    ERR_UNSETENV_ALPHA_CAR
} error_type_t;

char *get_error_msg(error_type_t type);

int print_error(char *cmd, char *msg, int nb);
int get_errno_error(char *cmd);

#endif // ERROR_TYPES_H
