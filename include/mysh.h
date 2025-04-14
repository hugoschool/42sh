/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** mysh.h
*/

#ifndef MYSH_H
    #define MYSH_H
    #define MAX_PATH 1024
    #define PATH "PATH"
    #define HOME "HOME"
    #define NLSPATH "NLSPATH"
    #define ENV_PATH "/usr/bin/env/"
    #define NLSPATH_PT1 "NLSPATH=/usr/share/locale/%L/LC_MESSAGES/%N.c"
    #define NLSPATH_PT2 "at:/usr/share/locale/%cl/LC_MESSAGES/%N.cat"
    #define NLSPATH_PRINT_PT1 "NLSPATH=/usr/share/locale/%L/LC_MESSAGES/%N.c"
    #define NLSPATH_PRINT_PT2 "at:/usr/share/locale/%l/LC_MESSAGES/%N.cat\n"
    #define CD "cd"
    #define EXIT "exit"
    #define SETENV "setenv"
    #define UNSETENV "unsetenv"
    #define ENV "env"
    #define HEREDOC "heredoc"
    #define CORE_DUMPED " (core dumped)\n"
    #define PROMPT "$> "

    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <linux/limits.h>
    #include "errors.h"
    #include "ast.h"
    #include "commands.h"
    #include "pipe.h"
    #include "structs.h"
    #include "utilities.h"

    #define PIPE '|'
    #define REDIR_OUT '>'
    #define REDIR_IN '<'

// args_parser.c //
int check_input(char *line, ssize_t read);
int parse_args(char *line);

// command_struct.c //
int set_redirection(simple_command_t *cmd, redirection_type_t type,
    char *file);

// path_handler.c //
void execute_command_path(char *args[]);

#endif //MYSH_H
