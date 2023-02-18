#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include "application.h"
#include "database.h"
#include "helper.h"

#define MAX_LENGTH 30

int isPasswordGenerated = 0;
char *password = NULL;
char lowest = '!';
char highest = '~';

int choose_login_register_option(void)
{
    int option = 0;

    while (option != 1 && option != 2 && option != 3)
    {
        printf("\n1. Register \n");
        printf("2. Login \n");
        printf("3. Exit \n");
        printf("\nChoose an option: ");

        scanf("%d", &option);

        // Flush the buffer
        char line[100];
        fgets(line, sizeof(line), stdin);
    }

    return option;
}

int choose_account_data_option(void)
{
    int option = 0;

    while (option != 1 && option != 2 && option != 3 && option != 4 && option != 5 && option != 6)
    {
        printf("\n1. Generate password \n");
        printf("2. Add account\n");
        printf("3. Edit account\n");
        printf("4. Delete account\n");
        printf("5. List all accounts\n");
        printf("6. Exit\n");
        printf("\nChoose an option : ");

        scanf("%d", &option);

        // Flush the buffer
        char line[100];
        fgets(line, sizeof(line), stdin);
    }

    return option;
}

void free_password()
{
    if (isPasswordGenerated)
    {
        free(password);
    }
}

int generate_password()
{
    // Get users preferences
    char *uppercase = get_string("\n\tDo you want to include uppercase letters? (y/n): ", 3);
    char *lowercase = get_string("\tDo you want to include lowercase letters? (y/n): ", 3);
    char *numbers = get_string("\tDo you want to include numbers? (y/n): ", 3);
    char *special_chars = get_string("\tDo you want to include special characters? (y/n): ", 3);

    // At least one char type need to be included
    if (strcmp(uppercase, "n") == 0 && strcmp(lowercase, "n") == 0 && strcmp(numbers, "n") == 0 && strcmp(special_chars, "n") == 0)
    {
        printf("\n\t\tNo password generated. At least one character type is required\n");

        // Free memory
        free_all(4, uppercase, lowercase, numbers, special_chars);

        return 1;
    }

    int *length = get_int("\n\tSpecify length: ");

    // Allocate memory for password
    password = malloc(sizeof(char) * *length + 1);
    memset(password, 0, *length + 1);

    for (int i = 0; i < *length; i++)
    {
        char character = (char)(rand() % (highest - lowest + 1) + lowest);

        // Escape uppercase letters
        if (strcmp(uppercase, "n") == 0 && (character >= 'A' && character <= 'Z'))
        {
            i--;
            continue;
        }

        // Escape lowercase letters
        if (strcmp(lowercase, "n") == 0 && (character >= 'a' && character <= 'z'))
        {
            i--;
            continue;
        }

        // Escape numbers
        if (strcmp(numbers, "n") == 0 && (character >= '0' && character <= '9'))
        {
            i--;
            continue;
        }

        // Escape special characters
        if (strcmp(special_chars, "n") == 0 &&
            ((character <= '/' || character >= '{') ||
             (character >= ':' && character <= '@') ||
             (character >= '[' && character <= '`')))
        {
            i--;
            continue;
        }

        password[i] = character;
    }

    password[*length] = '\0';

    // Update password flag
    isPasswordGenerated = 1;

    printf("\n\t\tGenerated password: %s\n", password);

    // Free memory
    free_all(5, length, uppercase, lowercase, numbers, special_chars);

    return 0;
}

int add_account_data(sqlite3 *db)
{
    Account *account = (Account *)malloc(sizeof(Account));

    account->site = get_string("\n\tSite: ", MAX_LENGTH);
    account->username = get_string("\tUsername: ", MAX_LENGTH);

    char *pass = get_string("\tPassword (enter * to use generated one): ", MAX_LENGTH);

    if (strcmp(pass, "*") == 0)
    {
        if (password == NULL)
        {
            printf("\n\t\tNo password generated. Account is not saved");

            // Free memory
            free_account(account);
            free(pass);

            return 1;
        }
        else
        {
            // Use generated password
            account->password = password;

            // Password is used, new one is needed and no freeing is needed
            isPasswordGenerated = 0;
        }
    }
    else
    {
        // Use users password
        account->password = pass;
    }

    // Get user's Id from session
    account->user_id = atoi(getenv("SESSION_ID"));

    save_account(db, account);

    // It is already freed if it is assigned to an account
    if (strcmp(pass, "*") == 0)
    {
        free(pass);
    }

    // Free memory
    free_account(account);

    return 0;
}

int delete_account(sqlite3 *db)
{
    // Get Id of account from user
    int *id = get_int("\n\tAccount id: ");

    delete_account_by_id(db, *id);

    // Free memory
    free(id);

    return 0;
}

int edit_account(sqlite3 *db)
{
    // Get Id of account from user
    int *id = get_int("\n\tAccount id: ");

    Account *account = get_account_by_id(db, *id);

    if (account == NULL)
    {
        free(id);
        return 1;
    }

    // Free for new data
    free(account->username);
    free(account->password);

    account->username = get_string("\tUsername: ", MAX_LENGTH);
    char *pass = get_string("\tPassword (enter * to use generated): ", MAX_LENGTH);

    if (strcmp(pass, "*") == 0)
    {
        if (password == NULL)
        {
            account->password = get_string("\tNo generated password. Enter another one: ", MAX_LENGTH);
        }
        else
        {
            // Use generated password
            account->password = password;
        }
    }
    else
    {
        account->password = pass;
    }

    update_account(db, account);

    // Free memory
    free_account(account);
    free(pass);
    free(id);

    return 0;
}

int list_all_accounts(sqlite3 *db)
{
    int user_id = atoi(getenv("SESSION_ID"));
    int size;

    Account *user_accounts = get_all_accounts(db, user_id, &size);

    // Finish if no accounts were found
    if (user_accounts == NULL)
    {
        return -1;
    }

    // Determine maximum column widths
    int max_id_width = strlen("Id"), max_site_width = strlen("Site"), max_username_width = strlen("Username"), max_password_width = strlen("Password");
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

    char *id_dashes = malloc(max_id_width + 1); // allocate memory for the character array, including space for the null terminator
    memset(id_dashes, '-', max_id_width);       // initialize all the characters to the dash character
    id_dashes[max_id_width] = '\0';             // add the null terminator to the end of the character array

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
    printf("\n\t+----+-%*s-+-%*s-+-%*s-+-%*s-+", max_id_width, id_dashes, max_site_width, site_dashes, max_username_width, username_dashes, max_password_width, password_dashes);
    printf("\n\t| %-2s | %-*s | %-*s | %-*s | %-*s |", "No", max_id_width, "Id", max_site_width, "Site", max_username_width, "Username", max_password_width, "Password");
    printf("\n\t+----+-%*s-+-%*s-+-%*s-+-%*s-+", max_id_width, id_dashes, max_site_width, site_dashes, max_username_width, username_dashes, max_password_width, password_dashes);

    for (int i = 0; i < size; i++)
    {
        printf("\n\t| %-2d | %-*d | %-*s | %-*s | %-*s |", i + 1, max_id_width, user_accounts[i].id, max_site_width, user_accounts[i].site, max_username_width, user_accounts[i].username, max_password_width, user_accounts[i].password);

        free(user_accounts[i].site);
        free(user_accounts[i].username);
        free(user_accounts[i].password);
    }

    printf("\n\t+----+-%*s-+-%*s-+-%*s-+-%*s-+", max_id_width, id_dashes, max_site_width, site_dashes, max_username_width, username_dashes, max_password_width, password_dashes);

    printf("%s", "\n");

    // Free memory
    free(user_accounts);
    free_all(4, id_dashes, site_dashes, username_dashes, password_dashes);

    return 0;
}

int login(sqlite3 *db)
{
    char *username = get_string("\n\tUsername: ", MAX_LENGTH);
    char *password = get_password("\tMaster password: ", MAX_LENGTH);

    // Get users password from database
    unsigned char *db_password = get_users_password(db, username);

// Hash master passwords if openssl/env.h is installed
#ifdef OPENSSL_EVP_H
    unsigned char *hash = malloc(EVP_MAX_MD_SIZE);
    unsigned int length = 0;

    hash_password(password, hash, &length);
#endif

// Compare passwords
#ifdef OPENSSL_EVP_H
    if (memcmp(db_password, hash, strlen((char *)db_password)) != 0)
    {
        printf("\n\t\tPassword are incorrect\n");
        exit(1);
    }
#else
    if (strcmp(db_password, password) != 0)
    {
        printf("\n\t\tPassword are incorrect\n");
        exit(1);
    }
#endif

    const int user_id = get_users_id(db, username);

    // Convert the integer to a string and store the result in the str buffer
    char user[3];
    sprintf(user, "%d", user_id);

    // Track session
    setenv("SESSION_ID", user, 1);

    printf("\n\t\tLogin successfull\n");

    // Free memory
    free_all(4, username, password, db_password, hash);

    return 0;
}

int register_user(sqlite3 *db)
{
    User *user = (User *)malloc(sizeof(User));

    user->first_name = get_string("\n\tFirst name: ", MAX_LENGTH);
    user->last_name = get_string("\tLast name: ", MAX_LENGTH);
    user->username = get_string("\tUsername: ", MAX_LENGTH);
    char *password = get_password("\tMaster password: ", MAX_LENGTH);

// Hash master passwords if openssl/env.h is installed
#ifdef OPENSSL_EVP_H
    unsigned char *hash = calloc(EVP_MAX_MD_SIZE + 1, sizeof(char));
    unsigned int length = 0;

    hash_password(password, hash, &length);

    user->password = hash;
#else
    user->password = password;
#endif

    int success = save_user(db, user);

// Free memory
#ifdef OPENSSL_EVP_H
    free(password);
#endif
    free_user(user);

    return success;
}
