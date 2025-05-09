/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** my_fg.c
*/

#include "mysh.h"

int my_fg(char *args[], int ac)
{
    pid_cur_job(FOREGROUND, args);
    return 0;
}
