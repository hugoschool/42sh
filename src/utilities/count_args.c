/*
** EPITECH PROJECT, 2025
** 42sh
** File description:
** count_args.c
*/

/**
 * @brief Counts the number of arguments in an array.
 *
 * @param args : The array of arguments to count.
 * @return : Number of arguments in the array.
 */
int count_args(char **args)
{
    int count = 0;

    if (!args)
        return 0;
    while (args[count])
        count++;
    return count;
}
