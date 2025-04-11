/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** pipe.h
*/

#ifndef PIPE_H
    #define PIPE_H
    #include "mysh.h"

// redirection.c //
int parse_redirections(char **tokens, simple_command_t *cmd);

// tokenize_with_quotes.c //
char **process_line(char *line, char **tokens, int tokens_max);
char **tokenize_with_quotes(char *line);

// tokenize.c //
int handle_quotes_token(token_line_t *tl, token_state_t *state, int i);
int tokenize(char *line, char **tokens);

#endif //PIPE_H
