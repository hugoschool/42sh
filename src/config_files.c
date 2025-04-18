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
 * @brief Check for an existing config file and execute it's content
 */
void setup_config_files(void)
{
    char *buf = NULL;
    size_t size = 0;
    FILE *fd = get_file_path(CONFIG_FILE, "r");

    if (!fd)
        return;
    while (getline(&buf, &size, fd) != -1) {
        if (!(strlen(buf) < 2 || (buf[0] == '#' && buf[1] == '#')))
            execute_config(buf);
    }
    fclose(fd);
    free(buf);
}
