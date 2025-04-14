/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** parser_ast.c
*/

#include "mysh.h"

/**
 * @brief Allocates memory for a token array.
 *
 * @param line : The command line being tokenized.
 * @return : Allocated token array, or NULL on error.
 */
static char **allocate_tokens(char *line)
{
    char **tokens = malloc(sizeof(char *) * (strlen(line) + 1));

    if (!tokens) {
        perror("malloc failed");
        return NULL;
    }
    return tokens;
}

/**
 * @brief Processes a line into tokens and validates syntax.
 *
 * @param line_copy : The command line to tokenize.
 * @param tokens : The array to store tokens.
 * @return : Token count on success, -1 on error.
 */
static int process_tokens(char *line_copy, char **tokens)
{
    int token_count = tokenize(line_copy, tokens);

    if (token_count < 0)
        return -1;
    if (validate_syntax(tokens) != 0)
        return -1;
    return token_count;
}

/**
 * @brief Frees tokens and line on error.
 *
 * @param tokens : Token array to free.
 * @param line_copy : Line copy to free.
 * @param token_count : Number of tokens to free.
 * @return : NULL to indicate error.
 */
static char *free_tokens_and_line(char **tokens, char *line_copy,
    int token_count)
{
    for (int i = 0; i < token_count; i++)
        free(tokens[i]);
    free(tokens);
    free(line_copy);
    return NULL;
}

/**
 * @brief Preprocesses a command line for parsing.
 *
 * Trims whitespace, duplicates the line, allocates token array, and tokenizes.
 *
 * @param line : The input command line.
 * @param line_copy : Pointer to store the duplicated line.
 * @param tokens : Pointer to store the token array.
 * @param token_count : Pointer to store the token count.
 * @return : 1 on success, 0 on error.
 */
int preprocess_line(char *line, char **line_copy, char ***tokens,
    int *token_count)
{
    char *aliased_line = replace_alias_line(line);

    if (aliased_line)
        line = aliased_line;
    line = trim_whitespace(line);
    if (!line)
        return 0;
    *line_copy = strdup(line);
    if (!*line_copy) {
        perror("strdup failed");
        return 0;
    }
    *tokens = allocate_tokens(line);
    if (!*tokens) {
        free(*line_copy);
        return 0;
    }
    *token_count = process_tokens(*line_copy, *tokens);
    if (*token_count < 0) {
        free_tokens_and_line(*tokens, *line_copy, 0);
        return 0;
    }
    return 1;
}

/**
 * @brief Parses a command line into an abstract syntax tree.
 *
 * @param line : The command line to parse.
 * @return : Root node of the AST, or NULL on error.
 */
ast_node_t *parse_line(char *line)
{
    char *line_copy = NULL;
    char **tokens = NULL;
    int token_count = 0;
    ast_node_t *root = NULL;
    int pos = 0;

    if (!preprocess_line(line, &line_copy, &tokens, &token_count))
        return NULL;
    root = parse_command_list(tokens, &pos, token_count);
    if (!root)
        return (ast_node_t *)free_tokens_and_line(tokens, line_copy,
        token_count);
    return root;
}
