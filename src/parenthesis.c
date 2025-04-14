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
int process_redirections_after_subshell(char **tokens, int *pos, int max_pos,
    ast_node_t *subshell_node)
{
    node_type_t type;

    while (*pos < max_pos && tokens[*pos] &&
    (tokens[*pos][0] == REDIR_IN || tokens[*pos][0] == REDIR_OUT)) {
        if (tokens[*pos][0] == REDIR_OUT)
            type = (tokens[*pos][1] == REDIR_OUT) ? NODE_REDIR_APPEND :
            NODE_REDIR_OUT;
        if (tokens[*pos][0] != REDIR_OUT)
            type = (tokens[*pos][1] == REDIR_IN) ? NODE_REDIR_HEREDOC :
            NODE_REDIR_IN;
        (*pos)++;
        if (*pos >= max_pos || !tokens[*pos]) {
            print_error(get_error_msg(ERR_NO_NAME_REDIRECTION), NULL, 0);
            free_ast(subshell_node);
            return 0;
        }
        add_redirection(subshell_node, type, tokens[*pos]);
        (*pos)++;
    }
    return 1;
}

static ast_node_t *parse_parenthesis(char **tokens, int *pos, int max_pos)
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
    if (!subshell_node)
        return (ast_node_t *)free_ast(inner_cmd);
    return (!process_redirections_after_subshell(tokens, pos, max_pos,
    subshell_node)) ? NULL : subshell_node;
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
 * Set up redirections for a subshell
 *
 * @param node The AST node containing the redirections
 * @param old_stdin Pointer to store the original input descriptor
 * @param old_stdout Pointer to store the original output descriptor
 * @return 0 on success, error code otherwise
 */
int setup_subshell_redirections(ast_node_t *node, int *old_stdin,
    int *old_stdout)
{
    if (!node->redirections)
        return 0;
    *old_stdin = dup(STDIN_FILENO);
    *old_stdout = dup(STDOUT_FILENO);
    if (*old_stdin == -1 || *old_stdout == -1) {
        perror("dup failed");
        return 1;
    }
    if (setup_redirections(node) != 0) {
        restore_redirections(*old_stdin, *old_stdout);
        return 1;
    }
    return 0;
}

/**
 * Creates a child process and executes the AST subtree
 *
 * @param node The AST node containing the subtree to be executed
 * @return PID of the child process, or -1 on error
 */
static pid_t fork_and_execute_ast(ast_node_t *node)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return -1;
    }
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        exit(execute_ast(node->left));
    }
    return pid;
}

/**
 * Executes a subshell
 *
 * @param node The AST node representing the subshell
 * @return Return code for subshell execution
 */
int execute_subshell(ast_node_t *node)
{
    pid_t pid;
    int status;
    int old_stdin = -1;
    int old_stdout = -1;
    int validation_result = validate_subshell_node(node);

    if (validation_result != 0)
        return validation_result;
    if (setup_subshell_redirections(node, &old_stdin, &old_stdout) != 0)
        return 1;
    pid = fork_and_execute_ast(node);
    if (pid == -1) {
        if (node->redirections)
            restore_redirections(old_stdin, old_stdout);
        return 1;
    }
    waitpid(pid, &status, 0);
    if (node->redirections)
        restore_redirections(old_stdin, old_stdout);
    return handle_wait_status(status);
}
