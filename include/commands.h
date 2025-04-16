/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** commands.h
*/

#ifndef COMMANDS_H
    #define COMMANDS_H
    #include "mysh.h"

// env //
int print_environment(void);

// executor //
int main_execute_command(char *line);

// my_alias //
int my_alias(char *args[], int count);
char *replace_alias_line(char *line);
alias_t **my_getaliases(alias_t **new_aliases);

// my_cd //
int my_cd(char *args[], int count);

// my_exit //
int my_exit(char *args[]);

// my_getenv //
void add_nlspath(char **environ, char *args[]);
char *my_getenv(const char *name);

// my_history_bang //
int my_history_bang(char *args[], int count);

// my_history //
int save_history(char *line);
int my_history(char *args[], int count);

// my_setenv //
int my_setenv(char *args[], int count);

// my_unsetenv //
int check_unsetenv(char *args[]);
int my_unsetenv(char *args[]);

#endif //COMMANDS_H
