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
    #define NLSPATH_PT1 "/usr/share/locale/%L/LC_MESSAGES/%N.c"
    #define NLSPATH_PT2 "at:/usr/share/locale/%cl/LC_MESSAGES/%N.cat"
    #define NLSPATH_PRINT_PT1 "/usr/share/locale/%L/LC_MESSAGES/%N.c"
    #define NLSPATH_PRINT_PT2 "at:/usr/share/locale/%l/LC_MESSAGES/%N.cat\n"
    #define ALIAS "alias"
    #define CD "cd"
    #define EXIT "exit"
    #define SETENV "setenv"
    #define UNSETENV "unsetenv"
    #define ENV "env"
    #define HEREDOC "heredoc"
    #define CORE_DUMPED " (core dumped)\n"
    #define PROMPT "$> "
    #define MULTI_PROMPT "...: "
    #define CONFIG_FILE ".42shrc"

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
    #define AND '&'
    #define BACKSLASH '\\'
    #define AND_OP "&&"
    #define OR_OP "||"
    #define OPEN_PAREN '('
    #define CLOSE_PAREN ')'
    #define OPEN_BRACKET '['
    #define CLOSE_BRACKET ']'
    #define OPEN_BRACE '{'
    #define CLOSE_BRACE '}'
    #define QUOTE '\''
    #define DBL_QUOTE '"'

// args_parser.c //
int check_input(char *line, ssize_t read);
int parse_args(char *line);

// command_struct.c //
int set_redirection(simple_command_t *cmd, redirection_type_t type,
    char *file);

// multiline.c //
int has_unclosed_quotes(const char *line, char *quote_type);
char *read_multiline_input(char *initial_line, int check_type, void *param);
int has_trailing_continuation(const char *line, int *is_operator);
int has_unclosed_brackets(const char *line, char *bracket_type);

// parenthesis.c //
int execute_subshell(ast_node_t *node);
ast_node_t *parse_command_or_subshell(char **tokens, int *pos, int max_pos);
int handle_wait_status(int wait_status);

// path_handler.c //
void execute_command_path(char *args[]);

// truth_table.c //
int execute_logical(ast_node_t *node, int op_is_and);
ast_node_t *parse_logical_expression(char **tokens, int *pos, int max_pos);
int handle_logical_operator(token_line_t *tl, token_state_t *state,
    int i);

// config_files.c //
void setup_config_files(void);

#endif //MYSH_H
