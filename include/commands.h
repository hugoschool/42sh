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
int create_fork(char *args[], int count);

// my_cd //
int my_cd(char *args[], int count);

// my_exit //
int my_exit(char *args[]);

// my_getenv //
void add_nlspath(char **environ, char *args[]);
char *my_getenv(const char *name);

// my_setenv //
int my_setenv(char *args[], int count);

// my_unsetenv //
int check_unsetenv(char *args[]);
int my_unsetenv(char *args[]);

#endif //COMMANDS_H
