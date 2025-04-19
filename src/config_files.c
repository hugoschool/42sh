/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** File that handle the startup and the parsing of the config file
*/

#include "mysh.h"

/**
 * @brief Parses a bindkey line from the .42shrc file
 *
 * @param line: Line to analyze
 * @return: 0 on success, 1 otherwise
 */
static int extract_tokens_bindkey(char *line, char **key_seq, char **func_name)
{
    char *token;
    char *saveptr;

    token = strtok_r(line, " \t", &saveptr);
    if (!token)
        return 1;
    token = strtok_r(NULL, " \t", &saveptr);
    if (!token)
        return 1;
    *key_seq = token;
    token = strtok_r(NULL, " \t", &saveptr);
    if (!token)
        return 1;
    *func_name = token;
    return 0;
}

static char *remove_quotes(char *str)
{
    if (str[0] == DBL_QUOTE && str[strlen(str) - 1] == DBL_QUOTE) {
        str[strlen(str) - 1] = '\0';
        str++;
    }
    return str;
}

static int parse_bindkey_line(char *line)
{
    char *key_seq = NULL;
    char *func_name = NULL;

    if (strncmp(line, BINDKEY, 7) != 0)
        return 1;
    if (extract_tokens_bindkey(line, &key_seq, &func_name))
        return 1;
    key_seq = remove_quotes(key_seq);
    func_name = remove_quotes(func_name);
    return handle_bindkey(key_seq, func_name);
}

static void execute_config(char *buf)
{
    int len = strlen(buf);

    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
    if (strncmp(buf, "bindkey", 7) == 0) {
        parse_bindkey_line(buf);
        return;
    }
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
