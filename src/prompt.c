/*
** EPITECH PROJECT, 2024
** B-PSU-200-LYN-2-1-42sh-hugo.arnal
** File description:
** File that handle prompt input
*/

#include "mysh.h"

static char *prompt_git(void)
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
            return NULL;
        }
        fclose(git);
        return line;
    }
    return NULL;
}

/**
 * @brief Displays the shell prompt if running in interactive mode.
 *
 * @return The allocated prompt.
 */
char *display_prompt(void)
{
    char *current_dir = calloc((PATH_MAX * 2) + 1, sizeof(char));
    char temp[PATH_MAX];
    char *line = NULL;

    strcpy(current_dir, COLOR_CYAN"-> ");
    getcwd(temp, PATH_MAX);
    strcat(current_dir, temp);
    line = prompt_git();
    if (line != NULL) {
        strcat(current_dir, COLOR_BLUE" git: "COLOR_RED);
        strcat(current_dir, &strstr(line, "ds/")[3]);
        free(line);
    }
    strcat(current_dir, COLOR_CYAN" $> "COLOR_NONE);
    if (isatty(STDIN_FILENO))
        return current_dir;
    return NULL;
}

/**
 * @brief Readline without printing if we're not in a tty.
 *
 * @param prompt : The prompt if we use readline.
 * @return The line from getline/readline.
 */
char *my_readline(char *prompt)
{
    size_t len = 0;
    ssize_t n = 0;
    char *line = NULL;

    if (!isatty(STDIN_FILENO)) {
        n = getline(&line, &len, stdin);
        if (n == -1) {
            free(prompt);
            return NULL;
        }
        if (n > 0 && line[n - 1] == '\n')
            line[n - 1] = '\0';
    } else
        line = readline(prompt);
    free(prompt);
    return line;
}
