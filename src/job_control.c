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
    if (sig == SIGSTOP)
        pid_cur_job(BACKGROUND, NULL);
}

/**
 * @breif fg
 */
static void foreground(llist_t **jobs, pid_t *shell, char *argv[])
{
    int argc = count_args(argv);

    if (*jobs == NULL) {
        printf("fg: pas de travail en cours.\n");
        return;
    }
    if (argc == 0) {
        kill(*shell, SIGTSTP);
        kill(*((pid_t *)(*jobs)->data), SIGCONT);
    }
}

/**
 * @breif ctrl + z
 */
static void background(llist_t **jobs, pid_t *shell)
{
    pid_t temp = getpid();

    if (temp == *shell)
        return;
    if (*jobs == NULL)
        *jobs = create_node(&temp);
    else
        cat_list(jobs, create_node(&temp), START);
    kill(temp, SIGTSTP);
    rl_on_new_line();
    rl_redisplay();
}

void pid_cur_job(enum jobs action, char *argv[])
{
    static llist_t *list = NULL;
    static pid_t shell = 0;

    if (action == GET_PID_SHELL) {
        shell = getpid();
        setpgid(shell, shell);
    }
    if (action == BACKGROUND)
        background(&list, &shell);
    if (action == FOREGROUND)
        foreground(&list, &shell, argv);
    return;
}
