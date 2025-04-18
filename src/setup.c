/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** setup.c
*/

#include "mysh.h"

/**
 * @brief Sets up the environment variables for the shell.
 *
 * Specifically, it sets the NLSPATH if not already defined.
 */
void setup_environment(void)
{
    char *args[] = {SETENV, NLSPATH,
    NLSPATH_PRINT_PT1 NLSPATH_PRINT_PT2, NULL};

    if (!my_getenv(NLSPATH))
        my_setenv(args, 2);
}

/**
 * @brief Sets up signal handlers for the shell.
 *
 * Ignores SIGINT, SIGQUIT, and SIGTSTP signals.
 */
void setup_signal_handlers(void)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}

/**
 * @brief Initializes readline and loads configured bindkeys
 */
void setup_bindkeys(void)
{
    init_default_bindkeys();
    rl_readline_name = "42sh";
    rl_attempted_completion_function = NULL;
}
