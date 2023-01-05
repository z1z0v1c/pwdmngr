#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "helper.h"

void free_all(int count, ...)
{
    va_list args;

    // initialize the variable argument list
    va_start(args, count);

    // iterate over the arguments and add them to the total
    for (int i = 0; i < count; i++)
    {
        char *ptr = va_arg(args, char *);
        if (ptr != NULL)
        {
            free(ptr);
        }
    }

    // clean up the variable argument list
    va_end(args);
}

void free_user(User *user)
{
    free(user->first_name);
    free(user->last_name);
    free(user->username);
    free(user->password);
    free(user);
}

void free_account(Account *account)
{
    free(account->site);
    free(account->username);
    free(account->password);
    free(account);
}

char *get_string(char *tip, int len)
{
    char *str = malloc(len + 1);
    printf("%s", tip);
    scanf("%s", str);

    return str;
}

int *get_int(char *tip)
{
    int *num = malloc(sizeof(int));
    printf("%s", tip);
    scanf("%d", num);

    return num;
}
