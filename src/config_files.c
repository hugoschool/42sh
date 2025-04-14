/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** File that handle the startup and the parsing of the config file
*/

#include "mysh.h"

static void execute_config(char *buf)
{
    int len = strlen(buf);

    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
    main_execute_command(buf);
}

/**
 * @brief Check for the config file in the $HOME dir
 *
 * @return Return the path of the config file in $HOME
 * else if the file does'nt exist or doesn't have the right acces, return NULL
 */
static char *file_in_home(void)
{
    char *path = NULL;
    char *home = my_getenv(HOME);

    if (!home)
        return NULL;
    path = malloc(sizeof(char) * (strlen(home) + 9));
    if (!path)
        return NULL;
    strcpy(path, home);
    strcat(path, "/");
    strcat(path, ".42shrc");
    if (access(path, R_OK) == 0)
        return path;
    free(path);
    return NULL;
}

/**
 * @brief Check for an existing config file and execute it's content
 */
void setup_config_files(void)
{
    char *buf = NULL;
    char *home_path = file_in_home();
    size_t size = 0;
    FILE *fd = home_path == NULL ? fopen(".42shrc", "r") :
        fopen(home_path, "r");

    if (!fd) {
        free(home_path);
        return;
    }
    while (getline(&buf, &size, fd) != -1) {
        if (!(strlen(buf) < 2 || (buf[0] == '#' && buf[1] == '#')))
            execute_config(buf);
    }
    fclose(fd);
    free(buf);
    free(home_path);
}
