/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** utilities.h
*/

#ifndef UTILITIES_H
    #define UTILITIES_H
    #include "mysh.h"

// bin_not_compatible.c //
void bin_not_compatible(char *arg);

// char_utilities.c //
char *trim_whitespace(char *line);
void strip_quotes(char *str);
void skip_whitespace(token_line_t *tl, int *i, token_state_t *state);

// frees.c //
void free_command(command_t *cmd);
void free_args(char **args);
char *free_ast(ast_node_t *node);
void free_tokens(char **tokens, int count);
char **free_tokens_on_error(char **tokens, int token_count);

// is.c //
int is_special_token(char c);
int is_valid_redirection_type(node_type_t type);
int is_logical_op(const char *line, int i);

// print_help.c //
void print_help(void);

#endif //UTILITIES_H
