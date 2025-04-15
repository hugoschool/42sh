/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** ast.h
*/

#ifndef AST_H
    #define AST_H
    #include "mysh.h"
    #include "structs.h"

// command_parser.c //
ast_node_t *parse_command(char **tokens, int *pos, int max_pos);
ast_node_t *parse_command_list(char **tokens, int *pos, int max_pos);

// create_ast.c //
ast_node_t *create_command_node(char **args);
ast_node_t *create_operator_node(node_type_t type, ast_node_t *left,
    ast_node_t *right);
void add_redirection(ast_node_t *node, node_type_t type, char *file);
int handle_heredoc(char *delimiter);

// execute_pipe.c //
int execute_pipe(ast_node_t *node);

// execute_ast.c //
int execute_ast(ast_node_t *node);
int execute_command(ast_node_t *node);
void restore_redirections(int old_stdin, int old_stdout);

// execute_builtins.c //
int handle_builtin_command(ast_node_t *node, int old_stdin, int old_stdout);

// parser_ast.c //
int preprocess_line(char *line, char **line_copy, char ***tokens,
    int *token_count);
ast_node_t *parse_line(char *line);

// pipeline_parser.c //
ast_node_t *parse_pipeline(char **tokens, int *pos, int max_pos);

// redirections.c //
int setup_redirections(ast_node_t *node);
void restore_redirections(int old_stdin, int old_stdout);

// validate_syntax.c //
int validate_syntax(char **tokens);
int validate_subshell_node(ast_node_t *node);

#endif //AST_H
