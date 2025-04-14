/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** path_handler.c
*/

#include "mysh.h"

/**
 * @brief Checks if a file is a directory and has execute permissions.
 *
 * @param arg : The file path to check.
 */
static void check_is_dir(char *arg)
{
    struct stat st;

    if (stat(arg, &st) == 0 && !S_ISDIR(st.st_mode) &&
    !(st.st_mode &(S_IXUSR | S_IXGRP | S_IXOTH)))
        exit(print_error(arg, get_error_msg(ERR_NOT_A_DIR), 1));
}

/**
 * @brief Executes a file if it's accessible.
 * Checks for existence, execute permissions, and compatible format.
 *
 * @param args : Arguments passed to the command.
 * @param environ : Environment variables.
 * @param pass : Flag to force checking the file.
 */
static void execute_file(char *args[], char **environ, int pass)
{
    if (args[0][0] == '/' || (args[0][0] == '.' && args[0][1] == '/') ||
    pass) {
        if (access(args[0], F_OK) != 0)
            exit(print_error(args[0], get_error_msg(ERR_NOT_FOUND), 1));
        check_is_dir(args[0]);
        if (access(args[0], X_OK) != 0)
            exit(print_error(args[0], get_error_msg(ERR_PERMISSION), 1));
        if (execve(args[0], args, environ) == -1)
            exit(get_errno_error(args[0]));
    }
}

/**
 * @brief Retrieves the value of the PATH environment variable.
 *
 * @return : A dynamically allocated string containing the PATH value, or NULL
 * if the variable is not found.
 * @note : This function allocates memory for the PATH value, which should be
 * freed after use.
 */
static char *get_path_value(void)
{
    char *path_value = my_getenv(PATH);

    if (!path_value)
        return NULL;
    path_value = strdup(path_value);
    if (!path_value) {
        perror("malloc");
        exit(1);
    }
    return path_value;
}

/**
 * @brief Builds a full path by combining directory and command.
 *
 * @param dir : The directory path.
 * @param cmd : The command name.
 * @param full_path : Buffer to store the result.
 */
static void build_full_path(const char *dir, const char *cmd, char *full_path)
{
    int dir_len;

    strncpy(full_path, dir, MAX_PATH);
    dir_len = strlen(full_path);
    if (dir_len > 0 && full_path[dir_len - 1] != '/')
        strncpy(full_path + dir_len, "/", 2);
    strncpy(full_path + strlen(full_path), cmd,
    MAX_PATH - strlen(full_path));
}

/**
 * @brief Searches for and executes a command in PATH directories.
 *
 * @param args : The command arguments.
 * @param environ : The environment variables.
 */
static void search_in_path(char *args[], char **environ)
{
    char *path_value;
    char full_path[MAX_PATH];
    char *saveptr;
    char *dir;

    path_value = get_path_value();
    if (!path_value)
        exit(print_error(args[0], get_error_msg(ERR_NOT_FOUND), 1));
    dir = strtok_r(path_value, ":", &saveptr);
    while (dir) {
        build_full_path(dir, args[0], full_path);
        if (access(full_path, X_OK) == 0) {
            args[0] = full_path;
            break;
        }
        dir = strtok_r(NULL, ":", &saveptr);
    }
    free(path_value);
    execve(args[0], args, environ);
}

/**
 * @brief Executes the given command by checking various paths.
 *
 * @param args : Arguments passed to the command.
 */
void execute_command_path(char *args[])
{
    extern char **environ;
    struct stat st;

    if (!environ || !args || !args[0])
        exit(1);
    if (args[0][0] == '\0')
        exit(print_error(get_error_msg(ERR_NOT_FOUND), NULL, 1));
    if (access(args[0], F_OK) == 0) {
        if (stat(args[0], &st) == 0 && S_ISDIR(st.st_mode))
            exit(print_error(args[0], get_error_msg(ERR_PERMISSION), 1));
        if (access(args[0], X_OK) != 0)
            exit(print_error(args[0], get_error_msg(ERR_PERMISSION), 1));
    }
    add_nlspath(environ, args);
    execute_file(args, environ, 0);
    if (!(args[0][0] == '/' || (args[0][0] == '.' && args[0][1] == '/')))
        search_in_path(args, environ);
    exit(print_error(args[0], get_error_msg(ERR_NOT_FOUND), 1));
}
