#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "helper.h"

// Free memory for all args 
void free_all(int count, ...)
{
    va_list args;

    // Initialize the variable argument list
    va_start(args, count);

    // Iterate over the arguments and add them to the total
    for (int i = 0; i < count; i++)
    {
        char *ptr = va_arg(args, char *);
        if (ptr != NULL)
        {
            free(ptr);
        }
    }

    // Clean up the variable argument list
    va_end(args);
}

// Free memory for an user
void free_user(User *user)
{
    free(user->first_name);
    free(user->last_name);
    free(user->username);
    free(user->password);
    free(user);
}

// Free memory for an account
void free_account(Account *account)
{
    free(account->site);
    free(account->username);
    free(account->password);
    free(account);
}

// Get text input from user
char *get_string(char *tip, int len)
{
    char *str = malloc(len + 1);
    printf("%s", tip);
    scanf("%s", str);

    return str;
}

// Get a number from user
int *get_int(char *tip)
{
    int *num = malloc(sizeof(int));
    printf("%s", tip);
    scanf("%d", num);

    return num;
}
