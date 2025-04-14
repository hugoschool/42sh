/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** structs.h
*/

#ifndef STRUCT_H
    #define STRUCT_H
    #include "mysh.h"

// AST //
/**
 * @brief Node types for the AST
 *
 * NODE_COMMAND : Simple command (with args)
 * NODE_PIPE : Pipe operator |
 * NODE_SEMICOLON : Semicolon operator ;
 * NODE_REDIR_OUT : Output redirection >
 * NODE_REDIR_IN : Input redirection <
 * NODE_REDIR_APPEND : Append redirection >>
 * NODE_REDIR_HEREDOC : Here document <<
 */
typedef enum {
    NONE,
    NODE_COMMAND,
    NODE_PIPE,
    NODE_AND,
    NODE_OR,
    NODE_SEMICOLON,
    NODE_REDIR_OUT,
    NODE_REDIR_IN,
    NODE_REDIR_APPEND,
    NODE_REDIR_HEREDOC,
    NODE_SUBSHELL
} node_type_t;

/**
 * @brief Structure for redirection information
 *
 * type : Type of redirection
 * file : File or string for redirection
 * next : Next redirection (for multiple redirections)
 */
typedef struct redir_s {
    node_type_t type;
    char *file;
    struct redir_s *next;
} redir_t;

/**
 * @brief Main AST node structure
 *
 * type : Type of node
 * args : Command arguments (NULL for non-command nodes)
 * redirections : Redirections for this command
 * left : Left child (or NULL)
 * right : Right child (or NULL)
 */
typedef struct ast_node_s {
    node_type_t type;
    char **args;
    redir_t *redirections;
    struct ast_node_s *left;
    struct ast_node_s *right;
} ast_node_t;


// COMMANDS //
/**
 * @brief Type for redirection
 *
 * REDIR_NONE :
 * REDIR_OUT_SIMPLE : >
 * REDIR_OUT_APPEND : >>
 * REDIR_IN_SIMPLE : <
 * REDIR_IN_HEREDOC : <<
 */
typedef enum redirection_type_e {
    REDIR_NONE,
    REDIR_OUT_SIMPLE,
    REDIR_OUT_APPEND,
    REDIR_IN_SIMPLE,
    REDIR_IN_HEREDOC
} redirection_type_t;

/**
 * @brief Structure for stock redirection
 *
 * type : type of redirection
 * file : file or content for the redirection
 */
typedef struct redirection_s {
    redirection_type_t type;
    char *file;
} redirection_t;

/**
 * @brief Structure for unique command (without pipe)
 *
 * args : command arguments
 * arg_count : nbr of arguments
 * in : input redirection
 * out : ouput redirection
 */
typedef struct simple_command_s {
    char **args;
    int arg_count;
    redirection_t in;
    redirection_t out;
} simple_command_t;

/**
 * @brief Structure for complex command (with pipes)
 *
 * commands : tab of simples commands
 * cmd_count : nbr of simples commands (nb of pipes + 1)
 */
typedef struct command_s {
    simple_command_t **commands;
    int cmd_count;
} command_t;

// TODO: docstrings
typedef struct alias_s {
    char *alias;
    char *command;
} alias_t;

// DATA //
/**
 * @brief Tokens states
 *
 * count : Nombre de tokens trouvés
 * in_quotes : Flag indiquant si on est dans des guillemets
 * in_token : Flag indiquant si on est dans un token
 * start : Position de début du token courant
 * quote_char : Caractère de citation actuel (' ou ")
 */
typedef struct token_state_s {
    int count;
    int in_quotes;
    int in_token;
    int start;
    char quote_char;
} token_state_t;

/**
 * @brief Structure for line manipulation parameters
 *
 * line : Ligne à tokeniser
 * tokens : Tableau de tokens résultant
 * tokens_max : Taille maximale du tableau de tokens
 */
typedef struct token_line_s {
    char *line;
    char **tokens;
    int tokens_max;
} token_line_t;

typedef struct env_data_s {
    char **old_env;
    char **new_env;
    const char *var;
    const char *val;
    int env_size;
    int var_len;
    int val_len;
    int var_exists;
} env_data_t;

typedef struct unset_data_s {
    char **old_env;
    char **new_env;
    const char *var;
    int env_size;
    int var_len;
    int found;
} unset_data_t;

#endif //STRUCT_H
