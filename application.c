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
#define LOWEST '!'
#define HIGHEST '~'

int isPasswordGenerated = 0;
char *password = NULL;

void free_password(void)
{
    if (isPasswordGenerated)
    {
        free(password);
    }
}

int generate_password(void)
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

    // Allocate memory for a password
    password = malloc(sizeof(char) * *length + 1);
    memset(password, 0, *length + 1);

    // Generate random numbers
    for (int i = 0; i < *length; i++)
    {
        char character = (char)(rand() % (HIGHEST - LOWEST + 1) + LOWEST);

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
    // Allocate memory for an account
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
            // Use a generated password
            account->password = password;

            // Password has been used, a new one and no freeing is needed
            isPasswordGenerated = 0;
        }
    }
    else
    {
        // Use the user's password
        account->password = pass;
    }

    // Get the user's Id from the session
    account->user_id = atoi(getenv("SESSION_ID"));

    save_account(db, account);

    // It has been already freed if it is assigned to an account
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
    // Get Id of the account from the user
    int *id = get_int("\n\tAccount id: ");

    delete_account_by_id(db, *id);

    // Free memory
    free(id);

    return 0;
}

int edit_account(sqlite3 *db)
{
    // Get Id of the account from the user
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

        // Free memory
        free(pass);
    }
    else
    {
        // Use the user's password
        account->password = pass;
    }

    update_account(db, account);

    // Free memory
    free_account(account);
    free(id);

    return 0;
}

int list_all_accounts(sqlite3 *db)
{
    // Get the user's Id from the session
    int user_id = atoi(getenv("SESSION_ID"));
    int size;

    Account *user_accounts = get_all_accounts(db, user_id, &size);

    // Finish if no accounts have been found
    if (user_accounts == NULL)
    {
        return -1;
    }

    print_accounts(user_accounts, size);

    // Free memory
    free(user_accounts);

    return 0;
}

int login(sqlite3 *db)
{
    char *username = get_string("\n\tUsername: ", MAX_LENGTH);
    char *password = get_password("\tMaster password: ", MAX_LENGTH);

    // Get users password from database
    unsigned char *db_password = get_users_password(db, username);

    if (db_password == NULL)
    {
        printf("\n\t\tIncorrect username\n");

        // Free memory
        free_all(3, username, password, db_password);

        return 1;
    }

    // Hash master passwords
    unsigned char *hash = malloc(EVP_MAX_MD_SIZE);
    unsigned int length = 0;

    hash_password(password, hash, &length);

    // Compare passwords
    if (memcmp(db_password, hash, strlen((char *)db_password)) != 0)
    {
        printf("\n\t\tIncorrect password\n");

        // Free memory
        free_all(4, username, password, db_password, hash);

        return 1;
    }

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
    // Allocate memory for an user
    User *user = (User *)malloc(sizeof(User));

    user->first_name = get_string("\n\tFirst name: ", MAX_LENGTH);
    user->last_name = get_string("\tLast name: ", MAX_LENGTH);
    user->username = get_string("\tUsername: ", MAX_LENGTH);
    char *password = get_password("\tMaster password: ", MAX_LENGTH);

    // Hash master passwords
    unsigned char *hash = calloc(EVP_MAX_MD_SIZE + 1, sizeof(char));
    unsigned int length = 0;

    hash_password(password, hash, &length);

    user->password = hash;

    int success = save_user(db, user);

    // Free memory
    free(password);
    free_user(user);

    return success;
}
