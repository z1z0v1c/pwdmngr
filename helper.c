#include <openssl/evp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "application.h"
#include "helper.h"

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

// Get single character from standard input - private
char get_char()
{
    // Declare and initialize variables
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

int *get_int(char *message)
{
    int *num;

    // Loop until number is entered
    while (1)
    {
        // Allocate memnory
        num = (int *)malloc(sizeof(int));

        // Print message
        printf("%s", message);

        // Scan the input and exit if a number is entered
        if (scanf("%d", num) == 1)
        {
            break;
        }

        // Free memory
        free(num);

        // Print an error message if number is not entered
        printf("\n\t\tNot a number. Please try again.\n");

        // Flush the buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    return num;
}

char *get_password(char *message, int len)
{
    // Allocate memory
    char *password = malloc(len + 1);
    int i = 0;

    // Print the message
    printf("%s", message);
    fflush(stdout);

    // Get characters and mask them with asterisks
    while (1)
    {
        // Get single character
        char c = get_char();

        // Exit if the user has pressed enter
        if (c == '\n' || c == '\r')
        {
            password[i] = 0;
            break;
        }

        // User has pressed backspace or delete
        if (c == 127 || c == 8)
        {
            if (i > 0)
            {
                printf("\b \b");
                i--;
            }
        }
        else // Save the character and mask it with an asterisk
        {
            password[i] = c;
            i++;
            printf("*");
            fflush(stdout);
        }
    }

    // Print new line
    printf("%s", "\n");

    return password;
}

char *get_string(char *message, int len)
{
    // Allocate memory
    char *str = malloc(len + 1);

    // Print the message
    printf("%s", message);

    // Scan the input
    scanf("%s", str);

    return str;
}

// Define hash function only if the openssl/env library is installed on the system
#ifdef OPENSSL_EVP_H
void hash_password(const char *password, unsigned char *hash, unsigned int *length)
{
    // Create a new EVP_MD_CTX object
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL)
    {
        printf("%s\n", "mdctx == NULL");
    }

    // Set the hashing algorithm
    const EVP_MD *md = EVP_sha256();

    // Initialize the hash context
    if (1 != EVP_DigestInit_ex(mdctx, md, NULL))
    {
        printf("%s\n", "EVP_DigestInit_ex error");
    }

    // Add the password string to the hash context
    if (1 != EVP_DigestUpdate(mdctx, password, strlen(password)))
    {
        printf("%s\n", "EVP_DigestUpdate error");
    }

    // Compute hash and store it in the "hash" buffer
    if (1 != EVP_DigestFinal_ex(mdctx, hash, length))
    {
        printf("%s\n", "EVP_DigestFinal_ex error");
    }

    // Free memory
    EVP_MD_CTX_free(mdctx);
}
#endif
