#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
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

// Get single character from standard input
char get_char()
{
    char character = 0;
    struct termios old_attributes = {0};

    // Get the terminal I/O attributes for the standard input 
    if (tcgetattr(0, &old_attributes) < 0)
    {
        perror("Can't get terminal I/O attributes.");
    }

    // Disable canonical input mode (line by line).
    old_attributes.c_lflag &= ~ICANON;

    // Disable the display of entered characters
    old_attributes.c_lflag &= ~ECHO;

    // Read only one character at the time
    old_attributes.c_cc[VMIN] = 1;

    // No timeout
    old_attributes.c_cc[VTIME] = 0;

    // Replace the terminal I/O attributes
    if (tcsetattr(0, TCSANOW, &old_attributes) < 0)
    {
        perror("Can't set terminal I/O attributes.");
    }

    // Read a single character
    if (read(0, &character, 1) < 0)
        perror("Can't read a character.");

    // Restore old attributes
    old_attributes.c_lflag |= ICANON;
    old_attributes.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old_attributes) < 0)
    {
        perror("Can't set terminal I/O attributes.");
    }

    return character;
}

// Get a number from the user
int *get_int(char *tip)
{
    int *num;
    while (1) {
        num = (int *)malloc(sizeof(int));

        printf("%s", tip);

        if (scanf("%d", num) == 1) {
            break;
        }

        free(num);
        printf("\n\t\tNot a number. Please try again.\n");

        // Flush the buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    return num;
}

// Get password input from user
char *get_password(char *tip, int len)
{
    char *password = malloc(len + 1);
    int i = 0;

    printf("%s", tip);
    fflush(stdout);

    // Get characters while replacing them with asterisks
    while (1)
    {
        char c = get_char();
        if (c == '\n' || c == '\r')
        {
            password[i] = 0;
            break;
        }
        if (c == 127 || c == 8)
        {
            if (i > 0)
            {
                printf("\b \b");
                i--;
            }
        }
        else
        {
            password[i] = c;
            i++;
            printf("*");
            fflush(stdout);
        }
    }

    return password;
}

// Get text input from user
char *get_string(char *tip, int len)
{
    char *str = malloc(len + 1);
    printf("%s", tip);
    scanf("%s", str);

    return str;
}
