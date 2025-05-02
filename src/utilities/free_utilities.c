/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell2-cannelle.gourdet
** File description:
** frees.c
*/

#include "mysh.h"

void free_env(void)
{
    extern char **environ;

    for (int i = 0; environ[i] != NULL; i++)
        free(environ[i]);
    free(environ);
}
