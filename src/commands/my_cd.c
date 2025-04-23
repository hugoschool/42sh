/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_cd.c
*/

#include "mysh.h"

extern char **environ;

/**
 * @brief Determines the new path for cd command.
 * Returns the appropriate path based on input or old directory.
 *
 * @param old_pwd : The previous working directory.
 * @param path : The desired path, can be NULL or "~".
 * @return : The path to navigate to.
 */
static char *get_new_path(char *old_pwd, char *path)
{
    char *args[] = {"setenv", "OLDPWD", old_pwd, NULL};

    if (!path || strcmp(path, "~") == 0) {
        path = my_getenv(HOME);
        if (!path || path[0] == '\0')
            exit(print_error(get_error_msg(ERR_NO_SUCH_FILE), NULL, 1));
    }
    if (strcmp(path, "-") == 0) {
        if (old_pwd[0] == '\0') {
            print_error(get_error_msg(ERR_NO_SUCH_FILE), NULL, 0);
            return NULL;
        }
        path = old_pwd;
        my_setenv(args, 2);
    }
    return path;
}

/**
 * @brief Validates and processes the arguments for the `cd` command.
 * Checks if too many arguments are provided and retrieves the
 * current working directory.
 *
 * @param args : The array of arguments.
 * @param count : The number of arguments.
 * @param old_pwd : The previous working directory.
 * @param cwd : The current working directory buffer.
 * @return : The new path to navigate to, or NULL in case of an error.
 */
static char *validate_cd_args(char *args[], int count, char *old_pwd,
    char *cwd)
{
    char *path = (count == 1) ? args[1] : NULL;

    if (args[1] && args[2]) {
        print_error(args[0], get_error_msg(ERR_TOO_MANY), 0);
        return NULL;
    }
    if (!getcwd(cwd, PATH_MAX)) {
        perror("getcwd failed");
        return NULL;
    }
    return get_new_path(old_pwd, path);
}

/**
 * @brief Changes the current working directory.
 * Checks if the path is accessible and performs the directory
 * change if possible.
 * Updates the old working directory if successful.
 *
 * @param path : The path to navigate to.
 * @param old_pwd : The previous working directory to be updated.
 * @param cwd : The current working directory buffer.
 * @return : 0 if the directory change is successful, 1 otherwise.
 */
static int change_directory(char *path, char *old_pwd, char *cwd)
{
    char new_pwd[PATH_MAX];
    char *set_pwd[] = {"setenv", "PWD", new_pwd, NULL};
    char *set_oldpwd[] = {"setenv", "OLDPWD", cwd, NULL};
    struct stat st;

    if (!path)
        return 1;
    if (access(path, F_OK) != 0)
        return print_error(path, get_error_msg(ERR_NO_SUCH_FILE), 1);
    if (stat(path, &st) == 0 && !S_ISDIR(st.st_mode))
        return print_error(path, get_error_msg(ERR_NOT_A_DIR), 1);
    if (access(path, X_OK) != 0 || chdir(path) != 0)
        return print_error(path, get_error_msg(ERR_PERMISSION), 1);
    strncpy(old_pwd, cwd, PATH_MAX);
    if (getcwd(new_pwd, PATH_MAX)) {
        my_setenv(set_pwd, 2);
        my_setenv(set_oldpwd, 2);
    }
    return 0;
}

/**
 * @brief Handles the `cd` command.
 * Uses `validate_cd_args` to check arguments and `change_directory`
 * to perform the change.
 * Maintains a static buffer for the previous working directory.
 *
 * @param args : The array of arguments.
 * @param count : The number of arguments.
 * @return : The result of `change_directory`,
 * 0 if successful, 1 otherwise.
 */
int my_cd(char *args[], int count)
{
    static char old_pwd[PATH_MAX] = "";
    char cwd[PATH_MAX];
    char *path = validate_cd_args(args, count, old_pwd, cwd);

    return change_directory(path, old_pwd, cwd);
}
