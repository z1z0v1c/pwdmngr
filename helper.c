#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char *get_string(char *tip, int len)
{
    char *str = malloc(len + 1);
    printf("%s", tip);
    scanf("%s", str);

    return str;
}