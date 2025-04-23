/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** mysh.h
*/

#ifndef MYSH_H
    #define MYSH_H
    #define PATH "PATH"
    #define HOME "HOME"
    #define NLSPATH "NLSPATH"
    #define ENV_PATH "/usr/bin/env/"
    #define NLSPATH_PT1 "/usr/share/locale/%L/LC_MESSAGES/%N.c"
    #define NLSPATH_PT2 "at:/usr/share/locale/%cl/LC_MESSAGES/%N.cat"
    #define NLSPATH_PRINT_PT1 "/usr/share/locale/%L/LC_MESSAGES/%N.c"
    #define NLSPATH_PRINT_PT2 "at:/usr/share/locale/%l/LC_MESSAGES/%N.cat\n"
    #define ALIAS "alias"
    #define BINDKEY "bindkey"
    #define CD "cd"
    #define EXIT "exit"
    #define HISTORY "history"
    #define HISTORY_BANG "!"
    #define SETENV "setenv"
    #define UNSETENV "unsetenv"
    #define WHERE "where"
    #define WHICH "which"
    #define ENV "env"
    #define HEREDOC "heredoc"
    #define CORE_DUMPED " (core dumped)\n"
    #define MULTI_PROMPT "...: "
    #define CONFIG_FILE ".42shrc"
    #define HISTORY_FILE ".42sh_history"

    #include <stdbool.h>
    #include <ctype.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <readline/readline.h>
    #include <readline/history.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <linux/limits.h>
    #include <dirent.h>
    #include <glob.h>
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

// bindkeys.c //
int handle_bindkey(const char *key_seq, const char *func_name);
void init_default_bindkeys(void);
int my_bindkey(char *args[], int count);
bindkey_mapping_t *get_bindkey_mappings(bindkey_mapping_t *new_mappings);
int parse_key_sequence(const char *seq);
rl_command_func_t *find_readline_function(const char *func_name);
void add_bindkey_mapping(const char *key_seq, const char *func_name);
void display_bindkey_help(void);

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
char *get_path_value(void);
void execute_command_path(char *args[], int wc_err);

// truth_table.c //
int execute_logical(ast_node_t *node, int op_is_and);
ast_node_t *parse_logical_expression(char **tokens, int *pos, int max_pos);
int handle_logical_operator(token_line_t *tl, token_state_t *state,
    int i);

// setup.c //
void setup_environment(void);
void setup_signal_handlers(void);
void setup_bindkeys(void);

// config_files.c //
void setup_config_files(void);

// prompt.c //
void display_prompt(void);

// wildcard //
int wildcard(ast_node_t *ast);

#endif //MYSH_H
