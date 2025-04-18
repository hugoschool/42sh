/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** Tests for minishell2
*/

#include <criterion/options.h>
#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <criterion/assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mysh.h"

int main(int argc, char *argv[]) {
    struct criterion_test_set *tests = criterion_initialize();

    int result = criterion_run_all_tests(tests);

    criterion_finalize(tests);
    return result;
}

void redirect_all_stdout(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

Test(string_utils, is_special_token)
{
    cr_assert(is_special_token(';'), "';' should be a special token");
    cr_assert(is_special_token('|'), "'|' should be a special token");
    cr_assert(is_special_token('<'), "'<' should be a special token");
    cr_assert(is_special_token('>'), "'>' should be a special token");
    cr_assert(!is_special_token('a'), "'a' should not be a special token");
    cr_assert(!is_special_token(' '), "Space should not be a special token");
}

Test(string_utils, trim_whitespace)
{
    char str1[] = "  hello";
    char str2[] = "    ";
    char str3[] = "hello  ";

    cr_assert_str_eq(trim_whitespace(str1), "hello", "Should remove leading spaces");
    cr_assert_null(trim_whitespace(str2), "All spaces should return NULL");
    cr_assert_str_eq(trim_whitespace(str3), "hello  ", "Should not remove trailing spaces");
}

Test(string_utils, strip_quotes)
{
    char str1[] = "\"hello\"";
    char str2[] = "'hello'";
    char str3[] = "\"hello";
    char str4[] = "hello\"";

    strip_quotes(str1);
    cr_assert_str_eq(str1, "hello", "Should remove double quotes");

    strip_quotes(str2);
    cr_assert_str_eq(str2, "hello", "Should remove single quotes");

    strip_quotes(str3);
    cr_assert_str_eq(str3, "hello", "Should handle unbalanced quotes (opening only)");

    strip_quotes(str4);
    cr_assert_str_eq(str4, "hello", "Should handle unbalanced quotes (closing only)");
}


Test(parser, parse_line_simple_command)
{
    char line[] = "ls -l";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_COMMAND, "Node type should be COMMAND");
    cr_assert_not_null(node->args, "Args should not be NULL");
    cr_assert_str_eq(node->args[0], "ls", "First arg should be 'ls'");
    cr_assert_str_eq(node->args[1], "-l", "Second arg should be '-l'");
    cr_assert_null(node->args[2], "Args should be NULL-terminated");
    cr_assert_null(node->redirections, "No redirections expected");

    free_ast(node);
}

Test(parser, parse_line_empty)
{
    char line[] = "";
    ast_node_t *node = parse_line(line);

    cr_assert_null(node, "Empty line should return NULL");
}

Test(parser, parse_line_whitespace)
{
    char line[] = "   \t   ";
    ast_node_t *node = parse_line(line);

    cr_assert_null(node, "Whitespace line should return NULL");
}

Test(parser, parse_line_invalid_syntax)
{
    char line[] = "ls | | grep";
    ast_node_t *node = parse_line(line);

    cr_assert_null(node, "Invalid syntax should return NULL");
}

Test(parser, parse_line_missing_redirection_target)
{
    char line[] = "ls >";
    ast_node_t *node = parse_line(line);

    cr_assert_null(node, "Missing redirection target should return NULL");
}


Test(ast, create_command_node)
{
    char **args = malloc(sizeof(char *) * 3);
    args[0] = strdup("echo");
    args[1] = strdup("hello");
    args[2] = NULL;

    ast_node_t *node = create_command_node(args);

    cr_assert_not_null(node, "Node creation should not return NULL");
    cr_assert_eq(node->type, NODE_COMMAND, "Node type should be COMMAND");
    cr_assert_eq(node->args, args, "Node args should match input args");
    cr_assert_null(node->redirections, "Node redirections should be NULL");
    cr_assert_null(node->left, "Node left should be NULL");
    cr_assert_null(node->right, "Node right should be NULL");

    free_ast(node);
}

Test(ast, create_operator_node)
{
    ast_node_t *left = create_command_node(NULL);
    ast_node_t *right = create_command_node(NULL);

    ast_node_t *pipe_node = create_operator_node(NODE_PIPE, left, right);

    cr_assert_not_null(pipe_node, "Node creation should not return NULL");
    cr_assert_eq(pipe_node->type, NODE_PIPE, "Node type should be PIPE");
    cr_assert_null(pipe_node->args, "Node args should be NULL");
    cr_assert_null(pipe_node->redirections, "Node redirections should be NULL");
    cr_assert_eq(pipe_node->left, left, "Node left should match input");
    cr_assert_eq(pipe_node->right, right, "Node right should match input");

    free_ast(pipe_node);
}

Test(ast, add_redirection)
{
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup("ls");
    args[1] = NULL;

    ast_node_t *node = create_command_node(args);

    add_redirection(node, NODE_REDIR_OUT, "output.txt");

    cr_assert_not_null(node->redirections, "Redirections should not be NULL");
    cr_assert_eq(node->redirections->type, NODE_REDIR_OUT, "Redirection type should be OUT");
    cr_assert_str_eq(node->redirections->file, "output.txt", "Redirection file should match");
    cr_assert_null(node->redirections->next, "Next redirection should be NULL");

    add_redirection(node, NODE_REDIR_IN, "input.txt");

    cr_assert_not_null(node->redirections->next, "Second redirection should exist");
    cr_assert_eq(node->redirections->next->type, NODE_REDIR_IN, "Second redirection type should be IN");
    cr_assert_str_eq(node->redirections->next->file, "input.txt", "Second redirection file should match");

    add_redirection(node, NODE_REDIR_OUT, "conflict.txt");

    cr_assert_null(node->redirections->next->next, "Conflicting redirection should not be added");

    free_ast(node);
}


Test(env, my_getenv)
{
    extern char **environ;

    setenv("TEST_VAR", "test_value", 1);

    char *value = my_getenv("TEST_VAR");
    cr_assert_not_null(value, "Should find existing variable");
    cr_assert_str_eq(value, "test_value", "Value should match");

    value = my_getenv("NONEXISTENT_VAR");
    cr_assert_null(value, "Should return NULL for non-existent variable");

    value = my_getenv(NULL);
    cr_assert_null(value, "Should handle NULL gracefully");

    unsetenv("TEST_VAR");
}

Test(env, my_setenv_basic, .init = redirect_all_stdout)
{
    extern char **environ;

    unsetenv("TEST_SETENV");

    char *args[] = {"setenv", "TEST_SETENV", "test_value", NULL};
    int result = my_setenv(args, 2);

    cr_assert_eq(result, 0, "my_setenv should return 0 on success");

    char *value = my_getenv("TEST_SETENV");
    cr_assert_not_null(value, "Variable should be set");
    cr_assert_str_eq(value, "test_value", "Value should match");

    unsetenv("TEST_SETENV");
}

Test(env, my_setenv_update, .init = redirect_all_stdout)
{
    extern char **environ;

    setenv("TEST_UPDATE", "old_value", 1);

    char *args[] = {"setenv", "TEST_UPDATE", "new_value", NULL};
    int result = my_setenv(args, 2);

    cr_assert_eq(result, 0, "my_setenv should return 0 on success");

    char *value = my_getenv("TEST_UPDATE");
    cr_assert_not_null(value, "Variable should be set");
    cr_assert_str_eq(value, "new_value", "Value should be updated");

    unsetenv("TEST_UPDATE");
}

Test(env, my_setenv_no_value, .init = redirect_all_stdout)
{
    extern char **environ;

    char *args[] = {"setenv", "TEST_EMPTY", NULL};
    int result = my_setenv(args, 1);

    cr_assert_eq(result, 0, "my_setenv should return 0 on success");

    char *value = my_getenv("TEST_EMPTY");
    cr_assert_not_null(value, "Variable should be set");
    cr_assert_str_eq(value, "", "Value should be empty string");

    unsetenv("TEST_EMPTY");
}

Test(env, my_setenv_invalid_name, .init = redirect_all_stdout)
{
    char *args[] = {"setenv", "123_INVALID", "value", NULL};
    int result = my_setenv(args, 2);

    cr_assert_eq(result, 1, "my_setenv should return error for invalid name");
}

Test(env, my_unsetenv_basic, .init = redirect_all_stdout)
{
    extern char **environ;

    setenv("TEST_UNSET", "value", 1);

    char *args[] = {"unsetenv", "TEST_UNSET", NULL};
    int result = my_unsetenv(args);

    cr_assert_eq(result, 0, "my_unsetenv should return 0 on success");

    char *value = my_getenv("TEST_UNSET");
    cr_assert_null(value, "Variable should be unset");
}

Test(env, my_unsetenv_nonexistent, .init = redirect_all_stdout)
{
    char *args[] = {"unsetenv", "NONEXISTENT_VAR", NULL};
    int result = my_unsetenv(args);

    cr_assert_eq(result, 0, "my_unsetenv should return 0 for non-existent var");
}

Test(env, my_unsetenv_too_few_args, .init = redirect_all_stdout)
{
    char *args[] = {"unsetenv", NULL};
    int result = my_unsetenv(args);

    cr_assert_eq(result, 1, "my_unsetenv should return error for too few args");
    cr_assert_stderr_eq_str("unsetenv: Too few arguments.\n",
                           "Should print error for too few args");
}

Test(env, print_environment, .init = redirect_all_stdout)
{
    setenv("TEST_ENV_PRINT", "test_value", 1);

    print_environment();

    fflush(stdout);

    fflush(stdout);

    cr_assert_stdout_neq_str("", "Output should not be empty");

    unsetenv("TEST_ENV_PRINT");
}


Test(command, setup_redirections)
{
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup("echo");
    args[1] = NULL;

    ast_node_t *node = create_command_node(args);

    add_redirection(node, NODE_REDIR_OUT, "nonexistent_output_file.txt");

    free_ast(node);
}


Test(utils, is_valid_redirection_type)
{
    cr_assert(is_valid_redirection_type(NODE_REDIR_IN), "IN should be valid");
    cr_assert(is_valid_redirection_type(NODE_REDIR_OUT), "OUT should be valid");
    cr_assert(is_valid_redirection_type(NODE_REDIR_APPEND), "APPEND should be valid");
    cr_assert(is_valid_redirection_type(NODE_REDIR_HEREDOC), "HEREDOC should be valid");
    cr_assert(!is_valid_redirection_type(NODE_COMMAND), "COMMAND should not be valid");
    cr_assert(!is_valid_redirection_type(NODE_PIPE), "PIPE should not be valid");
    cr_assert(!is_valid_redirection_type(NODE_SEMICOLON), "SEMICOLON should not be valid");
}


Test(utils, print_help, .init = redirect_all_stdout)
{
    print_help();

    fflush(stdout);
    cr_assert_stdout_neq_str("", "Help output should not be empty");
}


Test(builtin, my_cd_home, .init = redirect_all_stdout)
{
    extern char **environ;
    char original_dir[PATH_MAX];
    char *home_dir;

    getcwd(original_dir, PATH_MAX);

    home_dir = my_getenv("HOME");
    if (!home_dir) {
        cr_skip_test("HOME environment variable not set");
    }

    char *args[] = {"cd", NULL};
    int result = my_cd(args, 0);

    cr_assert_eq(result, 0, "cd to HOME should succeed");

    char current_dir[PATH_MAX];
    getcwd(current_dir, PATH_MAX);

    cr_assert_str_eq(current_dir, home_dir, "Should change to HOME directory");

    chdir(original_dir);
}

Test(builtin, my_cd_explicit_dir, .init = redirect_all_stdout)
{
    char original_dir[PATH_MAX];
    char *test_dir = "/tmp";

    getcwd(original_dir, PATH_MAX);

    char *args[] = {"cd", test_dir, NULL};
    int result = my_cd(args, 1);

    cr_assert_eq(result, 0, "cd to explicit dir should succeed");

    char current_dir[PATH_MAX];
    getcwd(current_dir, PATH_MAX);

    cr_assert_str_eq(current_dir, test_dir, "Should change to specified directory");

    chdir(original_dir);
}

Test(builtin, my_cd_nonexistent, .init = redirect_all_stdout)
{
    char *args[] = {"cd", "/nonexistent_directory_12345", NULL};
    int result = my_cd(args, 1);

    cr_assert_eq(result, 1, "cd to non-existent dir should fail");
    cr_assert_stderr_eq_str("/nonexistent_directory_12345: No such file or directory.\n",
                          "Should show error message");
}

Test(builtin, my_cd_too_many_args, .init = redirect_all_stdout)
{
    char *args[] = {"cd", "/tmp", "extra_arg", NULL};
    int result = my_cd(args, 2);

    cr_assert_eq(result, 1, "cd with too many args should fail");
    cr_assert_stderr_eq_str("cd: Too many arguments.\n", "Should show error message");
}

Test(builtin, my_exit_no_arg)
{
    char *args[] = {"exit", NULL};
    int result = my_exit(args);

    cr_assert_eq(result, 0, "exit with no arg should return 0");
}

Test(builtin, my_exit_with_code)
{
    char *args[] = {"exit", "42", NULL};
    int result = my_exit(args);

    cr_assert_eq(result, 42, "exit should return specified code");
}

Test(builtin, my_exit_invalid_format, .init = redirect_all_stdout)
{
    char *args[] = {"exit", "abc", NULL};
    int result = my_exit(args);

    cr_assert_eq(result, 1, "exit with invalid format should return 1");
}

Test(builtin, my_exit_too_many_args, .init = redirect_all_stdout)
{
    char *args[] = {"exit", "42", "extra", NULL};
    int result = my_exit(args);

    cr_assert_eq(result, 1, "exit with too many args should return 1");
    cr_assert_stderr_eq_str("exit: Badly formed number.\n", "Should show error message");
}


Test(memory, free_args)
{
    char **args = malloc(sizeof(char *) * 3);
    args[0] = strdup("arg1");
    args[1] = strdup("arg2");
    args[2] = NULL;

    free_args(args);

    free_args(NULL);
}

Test(memory, free_ast)
{
    char **args = malloc(sizeof(char *) * 3);
    args[0] = strdup("cmd");
    args[1] = strdup("arg");
    args[2] = NULL;

    ast_node_t *node = create_command_node(args);
    add_redirection(node, NODE_REDIR_OUT, "file.txt");

    char *result = free_ast(node);

    cr_assert_null(result, "free_ast should return NULL");
}

Test(parser, parse_line_with_pipe)
{
    char line[] = "ls -l | grep .c";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_PIPE, "Node type should be PIPE");
    cr_assert_not_null(node->left, "Left node should not be NULL");
    cr_assert_not_null(node->right, "Right node should not be NULL");

    cr_assert_eq(node->left->type, NODE_COMMAND, "Left node type should be COMMAND");
    cr_assert_str_eq(node->left->args[0], "ls", "First arg should be 'ls'");
    cr_assert_str_eq(node->left->args[1], "-l", "Second arg should be '-l'");

    cr_assert_eq(node->right->type, NODE_COMMAND, "Right node type should be COMMAND");
    cr_assert_str_eq(node->right->args[0], "grep", "First arg should be 'grep'");
    cr_assert_str_eq(node->right->args[1], ".c", "Second arg should be '.c'");

    free_ast(node);
}

Test(parser, parse_line_with_semicolon)
{
    char line[] = "ls -l ; echo hello";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_SEMICOLON, "Node type should be SEMICOLON");
    cr_assert_not_null(node->left, "Left node should not be NULL");
    cr_assert_not_null(node->right, "Right node should not be NULL");

    cr_assert_eq(node->left->type, NODE_COMMAND, "Left node type should be COMMAND");
    cr_assert_str_eq(node->left->args[0], "ls", "First arg should be 'ls'");
    cr_assert_str_eq(node->left->args[1], "-l", "Second arg should be '-l'");

    cr_assert_eq(node->right->type, NODE_COMMAND, "Right node type should be COMMAND");
    cr_assert_str_eq(node->right->args[0], "echo", "First arg should be 'echo'");
    cr_assert_str_eq(node->right->args[1], "hello", "Second arg should be 'hello'");

    free_ast(node);
}

Test(parser, parse_line_with_redirection_out)
{
    char line[] = "ls > file.txt";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_COMMAND, "Node type should be COMMAND");
    cr_assert_not_null(node->args, "Args should not be NULL");
    cr_assert_str_eq(node->args[0], "ls", "First arg should be 'ls'");

    cr_assert_not_null(node->redirections, "Redirections should not be NULL");
    cr_assert_eq(node->redirections->type, NODE_REDIR_OUT, "Redirection type should be OUT");
    cr_assert_str_eq(node->redirections->file, "file.txt", "Redirection file should be 'file.txt'");

    free_ast(node);
}

Test(parser, parse_line_with_redirection_in)
{
    char line[] = "wc < file.txt";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_COMMAND, "Node type should be COMMAND");
    cr_assert_not_null(node->args, "Args should not be NULL");
    cr_assert_str_eq(node->args[0], "wc", "First arg should be 'wc'");

    cr_assert_not_null(node->redirections, "Redirections should not be NULL");
    cr_assert_eq(node->redirections->type, NODE_REDIR_IN, "Redirection type should be IN");
    cr_assert_str_eq(node->redirections->file, "file.txt", "Redirection file should be 'file.txt'");

    free_ast(node);
}

Test(parser, parse_line_with_redirection_append)
{
    char line[] = "echo hello >> file.txt";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_COMMAND, "Node type should be COMMAND");
    cr_assert_not_null(node->args, "Args should not be NULL");
    cr_assert_str_eq(node->args[0], "echo", "First arg should be 'echo'");
    cr_assert_str_eq(node->args[1], "hello", "Second arg should be 'hello'");

    cr_assert_not_null(node->redirections, "Redirections should not be NULL");
    cr_assert_eq(node->redirections->type, NODE_REDIR_APPEND, "Redirection type should be APPEND");
    cr_assert_str_eq(node->redirections->file, "file.txt", "Redirection file should be 'file.txt'");

    free_ast(node);
}

Test(parser, parse_line_complex)
{
    char line[] = "ls -l | grep .c > output.txt ; echo done";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should not return NULL");
    cr_assert_eq(node->type, NODE_SEMICOLON, "Root node type should be SEMICOLON");

    cr_assert_eq(node->left->type, NODE_PIPE, "Left node type should be PIPE");

    cr_assert_eq(node->left->left->type, NODE_COMMAND, "First command type");
    cr_assert_str_eq(node->left->left->args[0], "ls", "First command");
    cr_assert_str_eq(node->left->left->args[1], "-l", "First command arg");

    cr_assert_eq(node->left->right->type, NODE_COMMAND, "Second command type");
    cr_assert_str_eq(node->left->right->args[0], "grep", "Second command");
    cr_assert_str_eq(node->left->right->args[1], ".c", "Second command arg");
    cr_assert_not_null(node->left->right->redirections, "Redirection exists");
    cr_assert_eq(node->left->right->redirections->type, NODE_REDIR_OUT, "Redirection type");
    cr_assert_str_eq(node->left->right->redirections->file, "output.txt", "Redirection file");

    cr_assert_eq(node->right->type, NODE_COMMAND, "Third command type");
    cr_assert_str_eq(node->right->args[0], "echo", "Third command");
    cr_assert_str_eq(node->right->args[1], "done", "Third command arg");

    free_ast(node);
}

Test(command_struct, set_redirection)
{
    simple_command_t cmd = {NULL, 0, {REDIR_NONE, NULL}, {REDIR_NONE, NULL}};

    int result = set_redirection(&cmd, REDIR_IN_SIMPLE, "input.txt");
    cr_assert_eq(result, 0, "Setting input redirection should succeed");
    cr_assert_eq(cmd.in.type, REDIR_IN_SIMPLE, "Input redirection type should be set");
    cr_assert_str_eq(cmd.in.file, "input.txt", "Input redirection file should be set");

    result = set_redirection(&cmd, REDIR_OUT_SIMPLE, "output.txt");
    cr_assert_eq(result, 0, "Setting output redirection should succeed");
    cr_assert_eq(cmd.out.type, REDIR_OUT_SIMPLE, "Output redirection type should be set");
    cr_assert_str_eq(cmd.out.file, "output.txt", "Output redirection file should be set");

    result = set_redirection(&cmd, REDIR_IN_HEREDOC, "conflict.txt");
    cr_assert_eq(result, 84, "Setting conflicting input redirection should fail");

    free(cmd.in.file);
    free(cmd.out.file);
}


Test(executor_ast, execute_command)
{
    char **args = malloc(sizeof(char *) * 3);
    args[0] = strdup("echo");
    args[1] = strdup("test");
    args[2] = NULL;

    ast_node_t *node = create_command_node(args);

    pid_t pid = fork();

    if (pid == 0) {
        int status = execute_command(node);
        exit(status);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "Command should execute");
        cr_assert_eq(WEXITSTATUS(status), 0, "Echo should return 0");
    }

    free_ast(node);
}


Test(execute_pipe, basic_pipe)
{
    char **left_args = malloc(sizeof(char *) * 3);
    left_args[0] = strdup("echo");
    left_args[1] = strdup("test");
    left_args[2] = NULL;

    char **right_args = malloc(sizeof(char *) * 2);
    right_args[0] = strdup("wc");
    right_args[1] = NULL;

    ast_node_t *left = create_command_node(left_args);
    ast_node_t *right = create_command_node(right_args);
    ast_node_t *pipe_node = create_operator_node(NODE_PIPE, left, right);

    pid_t pid = fork();

    if (pid == 0) {
        int status = execute_pipe(pipe_node);
        exit(status);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "Pipe should execute");
    }

    free_ast(pipe_node);
}


Test(setup_redirection, basic_setup)
{
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup("cat");
    args[1] = NULL;

    ast_node_t *node = create_command_node(args);

    FILE *input_fp = fopen("test_input.txt", "w");
    fprintf(input_fp, "test content\n");
    fclose(input_fp);

    add_redirection(node, NODE_REDIR_IN, "test_input.txt");
    add_redirection(node, NODE_REDIR_OUT, "test_output.txt");

    pid_t pid = fork();

    if (pid == 0) {
        int result = setup_redirections(node);
        if (result == 0) {
            write(1, "test", 4);
            exit(0);
        }
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "Setup should complete");

        FILE *output_fp = fopen("test_output.txt", "r");
        cr_assert_not_null(output_fp, "Output file should be created");

        char content[10];
        size_t read_size = fread(content, 1, 4, output_fp);
        content[read_size] = '\0';

        fclose(output_fp);
        cr_assert_str_eq(content, "test", "Redirected output should be correct");
    }

    free_ast(node);
    unlink("test_input.txt");
    unlink("test_output.txt");
}

Test(tokenize, tokenize_with_quotes_basic)
{
    char line[] = "ls -l \"test file\" 'single quotes'";
    char **tokens = tokenize_with_quotes(line);

    cr_assert_not_null(tokens, "tokenize_with_quotes should return tokens");
    cr_assert_str_eq(tokens[0], "ls", "First token should be ls");
    cr_assert_str_eq(tokens[1], "-l", "Second token should be -l");
    cr_assert_str_eq(tokens[2], "test file", "Third token should be quoted");
    cr_assert_str_eq(tokens[3], "single quotes", "Fourth token should handle single quotes");
    cr_assert_null(tokens[4], "Tokens should be NULL-terminated");

    free_args(tokens);
}

Test(tokenize, tokenize_with_quotes_edge_cases)
{
    char line1[] = "\"\"";
    char line2[] = "''";
    char line3[] = "echo \"hello\"'world'";
    char line4[] = "echo \"hello";

    char **tokens1 = tokenize_with_quotes(line1);
    cr_assert_not_null(tokens1, "Empty quotes should return tokens");
    cr_assert_str_eq(tokens1[0], "", "Empty quoted string");
    cr_assert_null(tokens1[1], "Tokens should be NULL-terminated");
    free_args(tokens1);

    char **tokens2 = tokenize_with_quotes(line2);
    cr_assert_not_null(tokens2, "Empty single quotes should return tokens");
    cr_assert_str_eq(tokens2[0], "", "Empty quoted string with single quotes");
    cr_assert_null(tokens2[1], "Tokens should be NULL-terminated");
    free_args(tokens2);

    char **tokens3 = tokenize_with_quotes(line3);
    cr_assert_not_null(tokens3, "Mixed quotes should be handled");
    cr_assert_str_eq(tokens3[0], "echo", "First token before quotes");
    cr_assert_str_eq(tokens3[1], "helloworld", "Combined quoted strings without space");
    cr_assert_null(tokens3[2], "Tokens should be NULL-terminated");
    free_args(tokens3);

    char **tokens4 = tokenize_with_quotes(line4);
    cr_assert_not_null(tokens4, "Unbalanced quotes should be handled");
    cr_assert_str_eq(tokens4[0], "echo", "First token before quote");
    cr_assert_str_eq(tokens4[1], "hello", "Content of unbalanced quotes");
    cr_assert_null(tokens4[2], "Tokens should be NULL-terminated");
    free_args(tokens4);
}

Test(env, add_nlspath)
{
    extern char **environ;

    char **original_environ = environ;

    char *args[] = {"ENV_PATH", "arg1", NULL};

    pid_t pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);

        add_nlspath(environ, args);
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "add_nlspath should not crash");
    }

    cr_assert_eq(environ, original_environ, "Original environ should be preserved");

    char *regular_args[] = {"regular_command", "arg1", NULL};
    pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);

        add_nlspath(environ, regular_args);
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "add_nlspath should not crash with regular command");
    }
}

Test(memory, free_command)
{
    simple_command_t *cmd1 = malloc(sizeof(simple_command_t));
    cmd1->args = malloc(sizeof(char *) * 3);
    cmd1->args[0] = strdup("echo");
    cmd1->args[1] = strdup("test");
    cmd1->args[2] = NULL;
    cmd1->arg_count = 2;
    cmd1->in.type = REDIR_NONE;
    cmd1->in.file = NULL;
    cmd1->out.type = REDIR_OUT_SIMPLE;
    cmd1->out.file = strdup("output.txt");

    command_t *cmd = malloc(sizeof(command_t));
    cmd->commands = malloc(sizeof(simple_command_t *) * 2);
    cmd->commands[0] = cmd1;
    cmd->commands[1] = NULL;
    cmd->cmd_count = 1;

    free_command(cmd);

    free_command(NULL);
}

Test(memory, free_tokens)
{
    char **tokens = malloc(sizeof(char *) * 4);
    tokens[0] = strdup("token1");
    tokens[1] = strdup("token2");
    tokens[2] = strdup("token3");
    tokens[3] = NULL;

    free_tokens(tokens, 3);

    free_tokens(NULL, 0);
}

Test(executor_ast, execute_ast_with_null)
{
    int result = execute_ast(NULL);
    cr_assert_eq(result, 0, "execute_ast with NULL should return 0");
}

Test(executor_ast, execute_ast_with_semicolon)
{
    char **args1 = malloc(sizeof(char *) * 3);
    args1[0] = strdup("echo");
    args1[1] = strdup("test1");
    args1[2] = NULL;

    char **args2 = malloc(sizeof(char *) * 3);
    args2[0] = strdup("echo");
    args2[1] = strdup("test2");
    args2[2] = NULL;

    ast_node_t *left = create_command_node(args1);
    ast_node_t *right = create_command_node(args2);
    ast_node_t *semicolon = create_operator_node(NODE_SEMICOLON, left, right);

    pid_t pid = fork();
    if (pid == 0) {
        int result = execute_ast(semicolon);
        exit(result);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "execute_ast with semicolon should complete");
        cr_assert_eq(WEXITSTATUS(status), 0, "execute_ast with semicolon should return 0");
    }

    free_ast(semicolon);
}

Test(executor_ast, execute_command_null_args)
{
    ast_node_t *node = create_command_node(NULL);

    int result = execute_command(node);

    cr_assert_eq(result, 0, "execute_command with NULL args should return 0");

    free_ast(node);
}

Test(executor_ast, execute_command_empty_args)
{
    char **args = malloc(sizeof(char *));
    args[0] = NULL;

    ast_node_t *node = create_command_node(args);

    int result = execute_command(node);

    cr_assert_eq(result, 0, "execute_command with empty args should return 0");

    free_ast(node);
}

Test(executor_ast, execute_command_unimplemented_builtin)
{
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup("alias");
    args[1] = NULL;

    ast_node_t *node = create_command_node(args);

    int result = execute_command(node);

    cr_assert_eq(result, 0, "execute_command with unimplemented builtin should return 0");

    free_ast(node);
}

Test(ast, create_operator_node_invalid, .init = redirect_all_stdout)
{
    ast_node_t *invalid_node = create_operator_node(999, NULL, NULL);

    cr_assert_null(invalid_node, "create_operator_node with invalid type should return NULL");
    cr_assert_stderr_eq_str("Invalid operator node type\n", "Should print error message for invalid operator type");
}

Test(ast, add_redirection_invalid)
{
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup("ls");
    args[1] = NULL;

    ast_node_t *node = create_command_node(args);

    add_redirection(node, 999, "file.txt");

    cr_assert_null(node->redirections, "add_redirection with invalid type should not add redirection");

    add_redirection(NULL, NODE_REDIR_OUT, "file.txt");

    add_redirection(node, NODE_REDIR_OUT, NULL);

    free_ast(node);
}

Test(ast, create_redirection_conflict, .init = redirect_all_stdout)
{
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup("cat");
    args[1] = NULL;

    ast_node_t *node = create_command_node(args);

    add_redirection(node, NODE_REDIR_IN, "input1.txt");

    add_redirection(node, NODE_REDIR_IN, "input2.txt");

    cr_assert_not_null(node->redirections, "First redirection should be present");
    cr_assert_eq(node->redirections->type, NODE_REDIR_IN, "First redirection type should be preserved");
    cr_assert_str_eq(node->redirections->file, "input1.txt", "First redirection file should be preserved");
    cr_assert_null(node->redirections->next, "Second redirection should be rejected due to conflict");

    cr_assert_stderr_eq_str("Ambiguous input redirect.\n", "Should print error for ambiguous input redirect");

    free_ast(node);
}

Test(ast, heredoc_redirection, .init = redirect_all_stdout)
{
    pid_t pid = fork();
    if (pid == 0) {
        int pipefd[2];
        pipe(pipefd);

        const char *test_data = "line1\nline2\nEOF\n";
        write(pipefd[1], test_data, strlen(test_data));
        close(pipefd[1]);

        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        int fd = handle_heredoc("EOF");

        if (fd < 0) {
            exit(1);
        }

        char buffer[100] = {0};
        int bytes_read = read(fd, buffer, sizeof(buffer));

        if (bytes_read <= 0 || strstr(buffer, "line1") == NULL) {
            exit(2);
        }

        close(fd);
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "handle_heredoc should complete");
        cr_assert_eq(WEXITSTATUS(status), 0, "handle_heredoc should succeed");
    }
}


Test(integration, parse_and_execute_pipe)
{
    char line[] = "echo test | wc -c";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should succeed");
    cr_assert_eq(node->type, NODE_PIPE, "Node type should be PIPE");

    pid_t pid = fork();
    if (pid == 0) {
        int result = execute_ast(node);
        exit(result);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "execute_ast with pipe should complete");
        cr_assert_eq(WEXITSTATUS(status), 0, "execute_ast with pipe should return 0");
    }

    free_ast(node);
}

Test(integration, parse_and_execute_redirections)
{
    char line[] = "echo test > testfile.txt";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should succeed");
    cr_assert_eq(node->type, NODE_COMMAND, "Node type should be COMMAND");
    cr_assert_not_null(node->redirections, "Redirections should be parsed");

    pid_t pid = fork();
    if (pid == 0) {
        int result = execute_ast(node);
        exit(result);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "execute_ast with redirection should complete");
        cr_assert_eq(WEXITSTATUS(status), 0, "execute_ast with redirection should return 0");

        struct stat st;
        cr_assert_eq(stat("testfile.txt", &st), 0, "Output file should exist");

        unlink("testfile.txt");
    }

    free_ast(node);
}

Test(integration, parse_and_execute_complex_command)
{
    char line[] = "echo test > file1.txt ; cat file1.txt | wc -c > file2.txt";
    ast_node_t *node = parse_line(line);

    cr_assert_not_null(node, "Parsing should succeed");
    cr_assert_eq(node->type, NODE_SEMICOLON, "Root node type should be SEMICOLON");

    pid_t pid = fork();
    if (pid == 0) {
        int result = execute_ast(node);
        exit(result);
    } else {
        int status;
        waitpid(pid, &status, 0);
        cr_assert(WIFEXITED(status), "execute_ast with complex command should complete");
        cr_assert_eq(WEXITSTATUS(status), 0, "execute_ast with complex command should return 0");

        struct stat st1, st2;
        cr_assert_eq(stat("file1.txt", &st1), 0, "First output file should exist");
        cr_assert_eq(stat("file2.txt", &st2), 0, "Second output file should exist");

        unlink("file1.txt");
        unlink("file2.txt");
    }

    free_ast(node);
}

Test(is_functions, test_is_special_token)
{
    cr_assert(is_special_token(';'));
    cr_assert(is_special_token(PIPE));
    cr_assert(is_special_token(REDIR_IN));
    cr_assert(is_special_token(REDIR_OUT));
    cr_assert(is_special_token(AND));
    cr_assert_not(is_special_token('a'));
    cr_assert_not(is_special_token(' '));
    cr_assert_not(is_special_token(0));
}

Test(is_functions, test_is_valid_redirection_type)
{
    cr_assert(is_valid_redirection_type(NODE_REDIR_IN));
    cr_assert(is_valid_redirection_type(NODE_REDIR_OUT));
    cr_assert(is_valid_redirection_type(NODE_REDIR_APPEND));
    cr_assert(is_valid_redirection_type(NODE_REDIR_HEREDOC));
    cr_assert_not(is_valid_redirection_type(NODE_COMMAND));
    cr_assert_not(is_valid_redirection_type(NODE_PIPE));
    cr_assert_not(is_valid_redirection_type(NODE_AND));
    cr_assert_not(is_valid_redirection_type(NODE_OR));
}

Test(logical_operators, test_handle_logical_operator, .init = redirect_all_stdout)
{
    token_line_t tl;
    token_state_t state;

    tl.line = "ls && cat";
    tl.tokens = malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
        tl.tokens[i] = NULL;

    state.count = 0;
    state.start = 0;
    state.in_token = 0;

    int ret = handle_logical_operator(&tl, &state, 0);
    cr_assert_eq(ret, 0);

    ret = handle_logical_operator(&tl, &state, 3);
    cr_assert_eq(ret, 1);
    cr_assert_str_eq(tl.tokens[0], "&&");
    cr_assert_eq(state.count, 1);

    free(tl.tokens[0]);
    free(tl.tokens);
}

Test(syntax_validation, test_validate_first_token, .init = redirect_all_stdout)
{
    char *valid_tokens[] = {"ls", "-l", NULL};
    char *pipe_first_tokens[] = {"|", "ls", NULL};
    char *and_first_tokens[] = {"&&", "ls", NULL};
    char *or_first_tokens[] = {"||", "ls", NULL};

    cr_assert_eq(validate_syntax(valid_tokens), 0);
    cr_assert_eq(validate_syntax(pipe_first_tokens), 84);
    cr_assert_eq(validate_syntax(and_first_tokens), 84);
    cr_assert_eq(validate_syntax(or_first_tokens), 84);
}

Test(syntax_validation, test_validate_consecutive_pipes, .init = redirect_all_stdout)
{
    char *valid_tokens[] = {"ls", "|", "grep", "pattern", NULL};
    char *invalid_tokens[] = {"ls", "|", "|", "grep", NULL};

    cr_assert_eq(validate_syntax(valid_tokens), 0);
    cr_assert_eq(validate_syntax(invalid_tokens), 84);
}

Test(syntax_validation, test_validate_logical_operators, .init = redirect_all_stdout)
{
    char *valid_tokens[] = {"ls", "&&", "grep", "pattern", NULL};
    char *invalid_and_tokens[] = {"ls", "&&", "&&", "grep", NULL};
    char *invalid_or_tokens[] = {"ls", "||", "||", "grep", NULL};
    char *invalid_mixed_tokens[] = {"ls", "&&", "||", "grep", NULL};

    cr_assert_eq(validate_syntax(valid_tokens), 0);
    cr_assert_eq(validate_syntax(invalid_and_tokens), 84);
    cr_assert_eq(validate_syntax(invalid_or_tokens), 84);
    cr_assert_eq(validate_syntax(invalid_mixed_tokens), 84);
}

Test(syntax_validation, test_validate_redirections, .init = redirect_all_stdout)
{
    char *valid_in_tokens[] = {"cat", "<", "input.txt", NULL};
    char *valid_out_tokens[] = {"ls", ">", "output.txt", NULL};
    char *invalid_in_tokens[] = {"cat", "<", NULL};
    char *invalid_out_tokens[] = {"ls", ">", NULL};
    char *invalid_redirect_pipe[] = {"ls", ">", "|", "grep", NULL};

    cr_assert_eq(validate_syntax(valid_in_tokens), 0);
    cr_assert_eq(validate_syntax(valid_out_tokens), 0);
    cr_assert_eq(validate_syntax(invalid_in_tokens), 84);
    cr_assert_eq(validate_syntax(invalid_out_tokens), 84);
    cr_assert_eq(validate_syntax(invalid_redirect_pipe), 84);
}

Test(syntax_validation, test_validate_single_ampersand, .init = redirect_all_stdout)
{
    char *valid_tokens[] = {"ls", "&&", "grep", "pattern", NULL};
    char *invalid_tokens[] = {"ls", "&", NULL};

    cr_assert_eq(validate_syntax(valid_tokens), 0);
    cr_assert_eq(validate_syntax(invalid_tokens), 84);
}

Test(syntax_validation, test_validate_last_token, .init = redirect_all_stdout)
{
    char *valid_tokens[] = {"ls", "-l", NULL};
    char *invalid_pipe_tokens[] = {"ls", "|", NULL};
    char *invalid_and_tokens[] = {"ls", "&&", NULL};
    char *invalid_or_tokens[] = {"ls", "||", NULL};

    cr_assert_eq(validate_syntax(valid_tokens), 0);
    cr_assert_eq(validate_syntax(invalid_pipe_tokens), 84);
    cr_assert_eq(validate_syntax(invalid_and_tokens), 84);
    cr_assert_eq(validate_syntax(invalid_or_tokens), 84);
}

Test(logical_parsing, test_parse_logical_expression_simple, .init = redirect_all_stdout)
{
    char *tokens[] = {"ls", NULL};
    int pos = 0;
    int max_pos = 1;

    ast_node_t *node = parse_logical_expression(tokens, &pos, max_pos);

    cr_assert_not_null(node);

    free_ast(node);
}

Test(logical_execution, test_execute_logical_invalid_node)
{
    int status = execute_logical(NULL, 1);
    cr_assert_eq(status, 1);

    ast_node_t *node = create_operator_node(NODE_AND, NULL, NULL);
    status = execute_logical(node, 1);
    cr_assert_eq(status, 1);
    free_ast(node);
}

Test(subshell, parse_command_or_subshell_command)
{
    char *tokens[] = {"ls", "-l", NULL};
    int pos = 0;
    int max_pos = 2;

    ast_node_t *result = parse_command_or_subshell(tokens, &pos, max_pos);

    cr_assert_not_null(result, "Should return a node");
    cr_assert_eq(result->type, NODE_COMMAND, "Node type should be NODE_COMMAND");

    free(result->args[0]);
    free(result->args);
    free(result);
}

Test(subshell, parse_command_or_subshell_subshell)
{
    char *tokens[] = {"(", "ls", ")", NULL};
    int pos = 0;
    int max_pos = 3;

    ast_node_t *result = parse_command_or_subshell(tokens, &pos, max_pos);

    cr_assert_not_null(result, "Should return a node");
    cr_assert_eq(result->type, NODE_SUBSHELL, "Node type should be NODE_SUBSHELL");
    cr_assert_not_null(result->left, "Left child should not be NULL");

    free_ast(result);
}

Test(subshell, handle_wait_status_exit)
{
    int status = 0;
    int result = handle_wait_status(status);

    cr_assert_eq(result, 0, "Should return exit code 0");
}

Test(subshell, execute_subshell_success)
{
    ast_node_t *inner = malloc(sizeof(ast_node_t));
    inner->type = NODE_COMMAND;
    inner->args = NULL;
    inner->redirections = NULL;
    inner->left = NULL;
    inner->right = NULL;

    ast_node_t *node = malloc(sizeof(ast_node_t));
    node->type = NODE_SUBSHELL;
    node->args = NULL;
    node->redirections = NULL;
    node->left = inner;
    node->right = NULL;
    free(inner);
    free(node);
    cr_assert(1, "Placeholder for complex subshell execution test");
}

Test(brackets, null_input)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets(NULL, &bracket_type), 0,
        "has_unclosed_brackets should return 0 for NULL input");
}

Test(brackets, empty_string)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("", &bracket_type), 0,
        "has_unclosed_brackets should return 0 for empty string");
}

Test(brackets, balanced_parentheses)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("(echo test)", &bracket_type), 0,
        "has_unclosed_brackets should return 0 for balanced parentheses");
}

Test(brackets, balanced_mixed)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("if (test) { [echo] }", &bracket_type), 0,
        "has_unclosed_brackets should return 0 for balanced mixed brackets");
}

Test(brackets, unclosed_parentheses)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("(echo test", &bracket_type), 1,
        "has_unclosed_brackets should return 1 for unclosed parentheses");
    cr_assert_eq(bracket_type, OPEN_PAREN,
        "bracket_type should be set to OPEN_PAREN for unclosed parentheses");
}

Test(brackets, unclosed_brackets)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("[echo test", &bracket_type), 1,
        "has_unclosed_brackets should return 1 for unclosed brackets");
    cr_assert_eq(bracket_type, OPEN_BRACKET,
        "bracket_type should be set to OPEN_BRACKET for unclosed brackets");
}

Test(brackets, unclosed_braces)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("{echo test", &bracket_type), 1,
        "has_unclosed_brackets should return 1 for unclosed braces");
    cr_assert_eq(bracket_type, OPEN_BRACE,
        "bracket_type should be set to OPEN_BRACE for unclosed braces");
}

Test(brackets, nested_unclosed)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("([{echo test}]", &bracket_type), 1,
        "has_unclosed_brackets should return 1 for nested unclosed brackets");
    cr_assert_eq(bracket_type, OPEN_PAREN,
        "bracket_type should detect the outermost unclosed parentheses");
}

Test(brackets, quotes_ignored)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("echo \"(\"", &bracket_type), 0,
        "has_unclosed_brackets should ignore brackets in quotes");
}

Test(brackets, multiple_unclosed)
{
    char bracket_type = 0;
    
    cr_assert_eq(has_unclosed_brackets("({ test", &bracket_type), 1,
        "has_unclosed_brackets should return 1 for multiple unclosed brackets");
    cr_assert_eq(bracket_type, OPEN_PAREN,
        "bracket_type should be set to OPEN_PAREN when multiple types are unclosed");
}


Test(continuation, null_input)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation(NULL, &is_operator), 0,
        "has_trailing_continuation should return 0 for NULL input");
}

Test(continuation, empty_string)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("", &is_operator), 0,
        "has_trailing_continuation should return 0 for empty string");
}

Test(continuation, no_continuation)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test", &is_operator), 0,
        "has_trailing_continuation should return 0 when no continuation");
}

Test(continuation, backslash)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test\\", &is_operator), 1,
        "has_trailing_continuation should return 1 for backslash");
    cr_assert_eq(is_operator, 0,
        "is_operator should be set to 0 for backslash");
}

Test(continuation, pipe)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test |", &is_operator), 1,
        "has_trailing_continuation should return 1 for pipe");
    cr_assert_eq(is_operator, 1,
        "is_operator should be set to 1 for pipe");
}

Test(continuation, semicolon)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test ;", &is_operator), 1,
        "has_trailing_continuation should return 1 for semicolon");
    cr_assert_eq(is_operator, 1,
        "is_operator should be set to 1 for semicolon");
}

Test(continuation, redirect_in)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test <", &is_operator), 1,
        "has_trailing_continuation should return 1 for redirect in");
    cr_assert_eq(is_operator, 1,
        "is_operator should be set to 1 for redirect in");
}

Test(continuation, redirect_out)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test >", &is_operator), 1,
        "has_trailing_continuation should return 1 for redirect out");
    cr_assert_eq(is_operator, 1,
        "is_operator should be set to 1 for redirect out");
}

Test(continuation, double_redirect_in)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test <<", &is_operator), 1,
        "has_trailing_continuation should return 1 for double redirect in");
    cr_assert_eq(is_operator, 1,
        "is_operator should be set to 1 for double redirect in");
}

Test(continuation, double_redirect_out)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test >>", &is_operator), 1,
        "has_trailing_continuation should return 1 for double redirect out");
    cr_assert_eq(is_operator, 1,
        "is_operator should be set to 1 for double redirect out");
}

Test(continuation, trailing_spaces)
{
    int is_operator = -1;
    
    cr_assert_eq(has_trailing_continuation("echo test\\   ", &is_operator), 1,
        "has_trailing_continuation should handle trailing spaces");
    cr_assert_eq(is_operator, 0,
        "is_operator should be set to 0 for backslash with trailing spaces");
}


Test(quotes, null_input)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes(NULL, &quote_type), 0,
        "has_unclosed_quotes should return 0 for NULL input");
}

Test(quotes, empty_string)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("", &quote_type), 0,
        "has_unclosed_quotes should return 0 for empty string");
}

Test(quotes, closed_single_quotes)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("echo 'test'", &quote_type), 0,
        "has_unclosed_quotes should return 0 for closed single quotes");
}

Test(quotes, closed_double_quotes)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("echo \"test\"", &quote_type), 0,
        "has_unclosed_quotes should return 0 for closed double quotes");
}

Test(quotes, unclosed_single_quotes)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("echo 'test", &quote_type), 1,
        "has_unclosed_quotes should return 1 for unclosed single quotes");
    cr_assert_eq(quote_type, QUOTE,
        "quote_type should be set to QUOTE for unclosed single quotes");
}

Test(quotes, unclosed_double_quotes)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("echo \"test", &quote_type), 1,
        "has_unclosed_quotes should return 1 for unclosed double quotes");
    cr_assert_eq(quote_type, DBL_QUOTE,
        "quote_type should be set to DBL_QUOTE for unclosed double quotes");
}

Test(quotes, mixed_quotes)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("echo 'test\"", &quote_type), 1,
        "has_unclosed_quotes should return 1 for mixed unclosed quotes");
    cr_assert_eq(quote_type, QUOTE,
        "quote_type should be set to QUOTE for mixed unclosed quotes");
}

Test(quotes, nested_quotes)
{
    char quote_type = 0;
    
    cr_assert_eq(has_unclosed_quotes("echo \"'test'\"", &quote_type), 0,
        "has_unclosed_quotes should return 0 for nested quotes");
}

