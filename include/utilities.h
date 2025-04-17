/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** utilities.h
*/

#ifndef UTILITIES_H
    #define UTILITIES_H
    #include "mysh.h"

    #define COLOR_CYAN "\033[1;36m"
    #define COLOR_BLUE "\033[1;34m"
    #define COLOR_RED "\033[1;31m"
    #define COLOR_NONE "\033[0;0m"

// char_utilities.c //
char *trim_whitespace(char *line);
void strip_quotes(char *str);
void skip_whitespace(token_line_t *tl, int *i, token_state_t *state);

// concat_args.c //
char *concat_args(char *args[]);

// count_args.c //
int count_args(char **args);

// count_lines_file.c //
long count_lines_file(FILE *fp);

// file_path.c //
char *is_file_in_home(char *file);
FILE *get_file_path(char *file, char *perms);

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
int is_valid_command_after_open_paren(char **tokens, int *pos,
    int max_pos);

// prepend.c //
char *prepend(char *line, char *pre, bool space);

// print_help.c //
void print_help(void);

// strisdigit.c //
int strisdigit(char *str);

#endif //UTILITIES_H
