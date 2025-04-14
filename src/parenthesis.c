/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** parenthesis.c
*/

#include "mysh.h"

/**
 * @brief Creates a subshell node in the AST.
 *
 * @param inner : The inner command AST node.
 * @return : Newly created subshell node, or NULL on error.
 */
static ast_node_t *create_subshell_node(ast_node_t *inner)
{
    ast_node_t *node = malloc(sizeof(ast_node_t));

    if (!node) {
        perror("malloc failed");
        return NULL;
    }
    node->type = NODE_SUBSHELL;
    node->args = NULL;
    node->redirections = NULL;
    node->left = inner;
    node->right = NULL;
    return node;
}

/**
 * @brief Validates that a command follows the opening parenthesis.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : 1 if valid, 0 if invalid with error message.
 */
static int is_valid_command_after_open_paren(char **tokens, int *pos,
    int max_pos)
{
    if (*pos >= max_pos || !tokens[*pos] || tokens[*pos][0] == CLOSE_PAREN)
        return print_error(get_error_msg(ERR_INVALID_NULL_COMMAND), NULL, 0);
    return 1;
}

/**
 * @brief Validates that a closing parenthesis exists.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : 1 if valid, 0 if invalid with error message.
 */
static int find_closing_parenthesis(char **tokens, int *pos, int max_pos)
{
    int paren_count = 1;
    int start_pos = *pos;

    while (*pos < max_pos && tokens[*pos]) {
        if (tokens[*pos][0] == OPEN_PAREN)
            paren_count++;
        if (tokens[*pos][0] != OPEN_PAREN && tokens[*pos][0] == CLOSE_PAREN)
            paren_count--;
        if (paren_count == 0)
            return 1;
        (*pos)++;
    }
    *pos = start_pos;
    return print_error(get_error_msg(ERR_MISSING_CLOSING_PAREN), NULL, 0);
}

/**
 * @brief Parses a subshell expression (commands inside parentheses).
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : AST node representing the subshell, or NULL on error.
 */
ast_node_t *parse_parenthesis(char **tokens, int *pos, int max_pos)
{
    ast_node_t *inner_cmd = NULL;
    ast_node_t *subshell_node = NULL;
    int end_pos;

    (*pos)++;
    if (!is_valid_command_after_open_paren(tokens, pos, max_pos))
        return NULL;
    end_pos = *pos;
    if (!find_closing_parenthesis(tokens, &end_pos, max_pos))
        return NULL;
    inner_cmd = parse_command_list(tokens, pos, end_pos);
    if (!inner_cmd)
        return NULL;
    *pos = end_pos + 1;
    subshell_node = create_subshell_node(inner_cmd);
    if (!subshell_node) {
        free_ast(inner_cmd);
        return NULL;
    }
    return subshell_node;
}

/**
 * @brief Parses a simple command or a subshell.
 *
 * @param tokens : Array of tokens being parsed.
 * @param pos : Pointer to current position in tokens.
 * @param max_pos : Maximum valid position in tokens.
 * @return : AST node representing the command or subshell, or NULL on error.
 */
ast_node_t *parse_command_or_subshell(char **tokens, int *pos, int max_pos)
{
    if (*pos < max_pos && tokens[*pos] && tokens[*pos][0] == OPEN_PAREN)
        return parse_parenthesis(tokens, pos, max_pos);
    else
        return parse_command(tokens, pos, max_pos);
}

/**
 * @brief Processes the wait status of a child process.
 *
 * @param wait_status : The status returned by waitpid.
 * @return : Normalized exit status.
 */
int handle_wait_status(int wait_status)
{
    int signal;

    if (WIFEXITED(wait_status))
        return WEXITSTATUS(wait_status);
    if (WIFSIGNALED(wait_status)) {
        signal = WTERMSIG(wait_status);
        return print_error(strsignal(signal),
        WCOREDUMP(wait_status) ? CORE_DUMPED : "", 128 + signal);
    }
    return 1;
}

/**
 * @brief Executes a subshell command in a child process.
 *
 * @param node : The AST node to execute.
 * @return : Status code of the execution.
 */
int execute_subshell(ast_node_t *node)
{
    pid_t pid;
    int status;

    if (!node || !node->left)
        return print_error(get_error_msg(ERR_INVALID_SUBSHELL), NULL, 1);
    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        exit(execute_ast(node->left));
    }
    waitpid(pid, &status, 0);
    return handle_wait_status(status);
}
