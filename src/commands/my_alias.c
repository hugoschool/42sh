/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** my_alias.c
** TODO: add docstrings
*/

#include "mysh.h"

int get_alias_size(alias_t **aliases)
{
    int i = 0;

    if (aliases == NULL)
        return 0;
    for (; aliases[i] != NULL; i++);
    return i;
}

alias_t **my_getaliases(alias_t **new_aliases)
{
    static alias_t **aliases = NULL;

    if (new_aliases != NULL)
        aliases = new_aliases;
    return aliases;
}

// command is a concatenate of all arguments
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

char *replace_command(char *command)
{
    char *new_command = get_alias_command(command);

    if (new_command)
        return new_command;
    return command;
}

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
