#include <openssl/evp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "application.h"
#include "helper.h"

static char get_char()
{
    char character = 0;
    struct termios old_attributes = {0};

    // Get the current terminal I/O attributes
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

    // Restore the old attributes
    old_attributes.c_lflag |= ICANON;
    old_attributes.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old_attributes) < 0)
    {
        perror("Can't set terminal I/O attributes.");
    }

    return character;
}

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

int *get_int(char *message)
{
    int *num;

    // Loop until number is entered
    while (1)
    {
        num = (int *)malloc(sizeof(int));

        printf("%s", message);

        // Scan the input and exit if a number is entered
        if (scanf("%d", num) == 1)
        {
            break;
        }

        free(num);

        printf("\n\t\tNot a number. Please try again.\n");

        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }

    return num;
}

char *get_password(char *message, int len)
{
    char *password = malloc(len + 1);
    int i = 0;

    printf("%s", message);
    fflush(stdout);

    while (1)
    {
        // Get single character
        char c = get_char();

        // Break the loop if the user has pressed enter
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

    printf("%s", "\n");

    return password;
}

char *get_string(char *message, int len)
{
    char *str = malloc(len + 1);

    printf("%s", message);

    scanf("%s", str);

    return str;
}

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

void print_accounts(Account *user_accounts, int size)
{
    int max_id_width = strlen("Id");
    int max_site_width = strlen("Site");
    int max_username_width = strlen("Username");
    int max_password_width = strlen("Password");

    // Determine maximum column widths
    for (int i = 0; i < size; i++)
    {
        int id_width = snprintf(NULL, 0, "%d", user_accounts[i].id);
        if (id_width > max_id_width)
        {
            max_id_width = id_width;
        }

        int site_width = strlen(user_accounts[i].site);
        if (site_width > max_site_width)
        {
            max_site_width = site_width;
        }

        int username_width = strlen(user_accounts[i].username);
        if (username_width > max_username_width)
        {
            max_username_width = username_width;
        }

        int password_width = strlen(user_accounts[i].password);
        if (password_width > max_password_width)
        {
            max_password_width = password_width;
        }
    }

    // Allocate memory for the arrays and initialize all the characters with dashes
    char *id_dashes = malloc(max_id_width + 1);
    memset(id_dashes, '-', max_id_width);       
    id_dashes[max_id_width] = '\0';

    char *site_dashes = malloc(max_site_width + 1);
    memset(site_dashes, '-', max_site_width);
    site_dashes[max_site_width] = '\0';

    char *username_dashes = malloc(max_username_width + 1);
    memset(username_dashes, '-', max_username_width);
    username_dashes[max_username_width] = '\0';

    char *password_dashes = malloc(max_password_width + 1);
    memset(password_dashes, '-', max_password_width);
    password_dashes[max_password_width] = '\0';

    // Print the table with dynamically adjusted column widths
    printf("\n\t+----+-%*s-+-%*s-+-%*s-+-%*s-+", 
            max_id_width, id_dashes, max_site_width, site_dashes, max_username_width,
            username_dashes, max_password_width, password_dashes);

    printf("\n\t| %-2s | %-*s | %-*s | %-*s | %-*s |",
            "No", max_id_width, "Id", max_site_width, "Site", max_username_width, 
            "Username", max_password_width, "Password");

    printf("\n\t+----+-%*s-+-%*s-+-%*s-+-%*s-+",
            max_id_width, id_dashes, max_site_width, site_dashes, max_username_width, 
            username_dashes, max_password_width, password_dashes);

    for (int i = 0; i < size; i++)
    {
        printf("\n\t| %-2d | %-*d | %-*s | %-*s | %-*s |", 
                i + 1, max_id_width, user_accounts[i].id, max_site_width, user_accounts[i].site,
                max_username_width, user_accounts[i].username, max_password_width, user_accounts[i].password);

        free(user_accounts[i].site);
        free(user_accounts[i].username);
        free(user_accounts[i].password);
    }

    printf("\n\t+----+-%*s-+-%*s-+-%*s-+-%*s-+",
            max_id_width, id_dashes, max_site_width, site_dashes, max_username_width,
            username_dashes, max_password_width, password_dashes);

    printf("%s", "\n");

    free_all(4, id_dashes, site_dashes, username_dashes, password_dashes);
}
