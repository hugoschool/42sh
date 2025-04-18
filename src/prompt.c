/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** File that handle prompt input
*/

#include "mysh.h"

static void prompt_git(void)
{
    FILE *git = fopen(".git/HEAD", "r");
    char *line = NULL;
    size_t n = 0;

    if (git != NULL) {
        getline(&line, &n, git);
        strstr(line, "\n")[0] = '\0';
        if (strstr(line, "ds/") == NULL) {
            fclose(git);
            free(line);
            return;
        }
        printf(COLOR_BLUE" git:%s%s", COLOR_RED,
            &(strstr(line, "ds/")[3]));
        fclose(git);
    }
    if (line != NULL)
        free(line);
}

/**
 * @brief Displays the shell prompt if running in interactive mode.
 */
void display_prompt(void)
{
    char current_dir[PATH_MAX];

    getcwd(current_dir, PATH_MAX);
    if (isatty(STDIN_FILENO)) {
        printf(COLOR_CYAN"-> %s", current_dir);
        prompt_git();
    }
}
