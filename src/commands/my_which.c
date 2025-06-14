/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_which.c
*/

#include "mysh.h"

/**
 * @brief Iterate directory in the PATH.
 *
 * @param path : The PATH env as a string.
 * @param arg : The argument to add at the end of the PATH string.
 * @param which : Is the command which or where?
*/
static bool print_file_path(char *path, char *arg, bool which)
{
    char *path_dir = NULL;
    char *saveptr = NULL;
    char *cmd_arg = NULL;
    bool found = false;

    if (!path)
        return false;
    path_dir = strtok_r(path, ":", &saveptr);
    while (path_dir) {
        cmd_arg = prepend(arg, path_dir, '/');
        if (access(cmd_arg, X_OK) == 0) {
            printf("%s\n", cmd_arg);
            found = true;
        }
        if (found && which)
            break;
        free(cmd_arg);
        path_dir = strtok_r(NULL, ":", &saveptr);
    }
    return found;
}

/**
 * @brief Display the buitlin message for where & which.
 *
 * @param args : The command that is a builtin.
 * @param which : Is the command which or where?
 * @return : Value of which
*/
static bool display_builtin_command(char *command, bool which)
{
    if (which)
        printf("%s: shell built-in command.\n", command);
    else
        printf("%s is a shell built-in\n", command);
    return which;
}

/**
 * @brief Iterate over each argument inside the array and determine if it's
 * a binary in the PATH or not.
 *
 * @param args : A NULL terminated arguments array.
 * @param count : The amount of arguments in the arguments array.
 * @param which : Is the command which or where?
 * @return 1 if we couldn't find ONE binary, 0 if we found ALL binaries
*/
static int iterate_args(char *args[], int count, bool which)
{
    char *path = NULL;
    int rv = 0;
    bool command_builtin = false;

    for (int i = 1; i < count + 1; i++) {
        path = get_path_value();
        command_builtin = is_builtin_command((char *[]){args[i], 0});
        if (command_builtin && display_builtin_command(args[i], which) == true)
            continue;
        if (!print_file_path(path, args[i], which) && !command_builtin)
            rv = print_error(args[i], get_error_msg(ERR_NOT_FOUND), 1);
        free(path);
    }
    return rv;
}

/**
 * @brief The builtin command for where.
 *
 * @param args : A NULL terminated arguments array.
 * @param count : The amount of arguments in the arguments array.
 * @return : Exit code for that builtin.
*/
static int my_where(char *args[], int count)
{
    return iterate_args(args, count, false);
}

/**
 * @brief The builtin command for where & which.
 *
 * @param args : A NULL terminated arguments array.
 * @param count : The amount of arguments in the arguments array.
 * @return : Exit code for that builtin.
*/
int my_which(char *args[], int count)
{
    if (count == 0)
        return print_error(args[0], get_error_msg(ERR_TOO_FEW), 1);
    if (strcmp(args[0], WHERE) == 0)
        return my_where(args, count);
    return iterate_args(args, count, true);
}
