/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** job_control.c
*/

#include "mysh.h"

static void free_jobs(llist_t *jobs, pid_t *pid)
{
    return;
}

void handle_sigstop(int sig)
{
    rl_on_new_line();
    rl_redisplay();
    if (sig == SIGSTOP)
        pid_cur_job(BACKGROUND, NULL);
}

/**
 * @breif fg
 */
static void foreground(llist_t **jobs, pid_t *pid, char *argv[])
{
    if (*jobs == NULL)
        printf("fg: pas de travail en cours.\n");
}

/**
 * @breif ctrl + z
 */
static void background(llist_t **jobs, pid_t *shell)
{
    pid_t temp = getpid();

    if (temp == *shell)
        return;
    if (jobs == NULL)
        *jobs = create_node(&temp);
}

void set_pid_shell(llist_t **jobs, pid_t *shell)
{
    *shell = getpid();
    (void)jobs;
}

void pid_cur_job(enum jobs action, char *argv[])
{
    static llist_t *list_job = NULL;
    static pid_t shell = 0;

    if (action == GET_PID_SHELL)
        shell = getpid();
    if (action == BACKGROUND)
        background(&list_job, &shell);
    if (action == FOREGROUND)
        foreground(&list_job, &shell, argv);
    return;
}
