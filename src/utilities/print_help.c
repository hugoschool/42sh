/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** print_help.c
*/

#include "mysh.h"

/**
 * @brief Prints information about shell features.
 */
void print_help_features(void)
{
    printf("\nFEATURES\n");
    printf(" - Command execution (from PATH or absolute path)\n");
    printf(" - Inhibitors (quotes: ' ')\n");
    printf(" - Globbings (*, ?, [, ])\n");
    printf(" - Job control (&, fg, bg)\n");
    printf(" - Backticks (`)\n");
    printf(" - Parentheses support ( and )\n");
    printf(" - Local and environment variables\n");
    printf(" - Special variables (term, precmd, cwdcmd, cwd, ignoreof)\n");
    printf(" - Command history (!)\n");
    printf(" - Aliases (example: alias ls \"ls --color\")\n");
    printf(" - Line edition (multiline, dynamic rebinding)\n");
    printf(" - Dynamic auto-completion\n");
    printf(" - Scripting support\n");
    printf(" - Error handling\n");
}

/**
 * @brief Prints the help message with usage information.
 */
void print_help(void)
{
    printf("USAGE\n");
    printf(" ./42sh\n");
    printf("\nDESCRIPTION\n");
    printf(" UNIX command interpreter based on TCSH\n\n");
    printf("BUILTINS\n");
    printf(" cd [directory] : Change current directory\n");
    printf(" env : Display environment variables\n");
    printf(" setenv [var] [value] : Set environment variable\n");
    printf(" unsetenv [var] : Remove environment variable\n");
    printf(" alias [name] [value] : Create command alias\n");
    printf(" history : Display command history\n");
    printf(" fg [job_id] : Bring job to foreground\n");
    printf(" bg [job_id] : Resume job in background\n");
    printf(" exit : Exit the shell\n");
    print_help_features();
}
