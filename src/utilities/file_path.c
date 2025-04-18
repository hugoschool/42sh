/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** is_file_in_home.c
*/

#include "mysh.h"

/**
 * @brief Check for a file in the $HOME dir
 *
 * @param file : Name of the file.
 * @return Return the path of the file in $HOME,
 * return NULL if no access or doesn't exist.
 */
char *is_file_in_home(char *file)
{
    char *path = NULL;
    char *home = my_getenv(HOME);

    if (!home)
        return NULL;
    path = malloc(sizeof(char) * (strlen(home) + strlen(file) + 2));
    if (!path)
        return NULL;
    strcpy(path, home);
    strcat(path, "/");
    strcat(path, file);
    if (access(path, R_OK) == 0)
        return path;
    free(path);
    return NULL;
}

/**
 * @brief Opens the correct file descriptors, either in $HOME
 * or in the current folder.
 *
 * @param file : Name of the file.
 * @param perms : Permission given to fopen.
 * @return Returns a file pointer to the file.
 */
FILE *get_file_path(char *file, char *perms)
{
    char *home_path = is_file_in_home(file);
    FILE *fd = home_path == NULL ? fopen(file, perms) :
        fopen(home_path, perms);

    free(home_path);
    return fd;
}
