/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_alias.c
*/

#include "mysh.h"

/**
 * @brief Gets the size of the aliases array.
 *
 * @param aliases : A NULL terminated aliases array.
 * @return : The size of the aliases array.
*/
int get_alias_size(alias_t **aliases)
{
    int i = 0;

    if (aliases == NULL)
        return 0;
    for (; aliases[i] != NULL; i++);
    return i;
}

/**
 * @brief Retrieves the aliases OR sets new aliases.
 *
 * @param new_aliases : A NULL terminated aliases array.
 * @return : The aliases array.
*/
alias_t **my_getaliases(alias_t **new_aliases)
{
    static alias_t **aliases = NULL;

    if (new_aliases != NULL)
        aliases = new_aliases;
    return aliases;
}

/**
 * @brief Retrieves the aliases OR sets new aliases.
 *
 * @param alias : The alias.
 * @param command : The command you want to replace for the aliases.
 * @return : The new aliases array.
*/
alias_t **my_setaliases(char *alias, char *command)
{
    alias_t **aliases = my_getaliases(NULL);
    int current_size = get_alias_size(aliases);

    if (!command)
        return NULL;
    aliases = realloc(aliases, sizeof(alias_t *) * (current_size + 2));
    if (!aliases)
        return NULL;
    aliases[current_size] = malloc(sizeof(alias_t));
    if (!aliases[current_size])
        return NULL;
    aliases[current_size]->alias = strdup(alias);
    aliases[current_size]->command = command;
    aliases[current_size + 1] = NULL;
    return my_getaliases(aliases);
}

/**
 * @brief Gets the alias command from the aliases array.
 *
 * @param alias : The alias you want to check if present in aliases array.
 * @return : If found, the command of the alias from aliases array.
 * If not, NULL.
*/
static char *get_alias_command(char *alias)
{
    alias_t **aliases = my_getaliases(NULL);

    if (!alias || !aliases)
        return NULL;
    for (int i = 0; aliases[i] != NULL; i++) {
        if (strcmp(aliases[i]->alias, alias) == 0)
            return aliases[i]->command;
    }
    return NULL;
}

/**
 * @brief Replaces the given string by the command of the alias.
 *
 * @param cmd : The string to be replaced.
 * @return : If found, newly replaced aliased command. If not, NULL.
*/
char *replace_alias_line(char *cmd)
{
    alias_t **alias = my_getaliases(NULL);
    char *str = NULL;
    char *temp = NULL;

    if (!alias)
        return NULL;
    for (int i = 0; alias[i] != NULL; i++) {
        temp = prepend(" ", alias[i]->alias, 0);
        if (!temp)
            return NULL;
        if (strcmp(cmd, alias[i]->alias) == 0 ||
            strncmp(cmd, temp, strlen(temp)) == 0) {
            str = prepend(&cmd[strlen(alias[i]->alias)], alias[i]->command, 0);
            break;
        }
        free(temp);
    }
    if (!str)
        return NULL;
    return str;
}

/**
 * @brief The builtin command for aliases.
 *
 * @param args : A NULL terminated arguments array.
 * @param count : The amount of arguments in the arguments array.
 * @return : Exit code for that builtin.
*/
int my_alias(char *args[], int count)
{
    alias_t **aliases = my_getaliases(NULL);
    char *command = NULL;

    if (count == 0 && aliases != NULL) {
        for (int i = 0; aliases[i] != NULL; i++)
            printf("%s\t%s\n", aliases[i]->alias, aliases[i]->command);
    }
    if (count == 1 && aliases != NULL) {
        command = get_alias_command(args[1]);
        if (command)
            printf("%s\n", command);
    }
    if (count >= 2) {
        if (!my_setaliases(args[1], concat_args(&args[2])))
            return 1;
    }
    return 0;
}
