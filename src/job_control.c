/*
** EPITECH PROJECT, 2025
** B-PSU-200-LYN-2-1-minishell1-cannelle.gourdet
** File description:
** job_control.c
*/

#include "mysh.h"

void handle_sigstop(int sig)
{
    if (sig == SIGSTOP)
        pid_cur_job(add_to_sleep_proc);
}

void reactivate_shell(llist_t *job, pid_t *pid)
{

}

void free_jobs(llist_t *jobs, pid_t *pid)
{

}

void add_to_sleep_proc(llist_t *jobs, pid_t *shell)
{
    pid_t temp = getpid();

    if (temp == *shell)
        return;
    if (jobs == NULL)
        jobs = create_node(&temp);
}

void set_pid_shell(llist_t *jobs, pid_t *shell)
{
    *shell = getpid();
    (void)jobs;
}

void pid_cur_job(void (*ptr_func)(llist_t *, pid_t *))
{
    static llist_t *list_job = NULL;
    static pid_t shell = 0;

    ptr_func(list_job, &shell);
}
