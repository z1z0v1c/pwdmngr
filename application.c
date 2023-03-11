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

int is_password_generated = 0;
char *generated_password = NULL;

void free_password(void)
{
    if (is_password_generated)
    {
        free(generated_password);
    }
}

int generate_password(void)
{
    char *include_uppercase = get_string("\n\tDo you want to include uppercase letters? (y/n): ", 3);
    char *include_lowercase = get_string("\tDo you want to include lowercase letters? (y/n): ", 3);
    char *numbers = get_string("\tDo you want to include numbers? (y/n): ", 3);
    char *special_chars = get_string("\tDo you want to include special characters? (y/n): ", 3);

    if (strcmp(include_uppercase, "n") == 0 &&
        strcmp(include_lowercase, "n") == 0 &&
        strcmp(numbers, "n") == 0 &&
        strcmp(special_chars, "n") == 0)
    {
        printf("\n\t\tNo password has been generated. At least one character type has to be included\n");

        free_all(4, include_uppercase, include_lowercase, numbers, special_chars);

        return 1;
    }

    int *length = get_int("\n\tSpecify length: ");

    generated_password = malloc(sizeof(char) * *length + 1);
    memset(generated_password, 0, *length + 1);

    // Generate random characters for the password
    for (int i = 0; i < *length; i++)
    {
        char character = (char)(rand() % (HIGHEST - LOWEST + 1) + LOWEST);

        // Escape uppercase letters
        if (strcmp(include_uppercase, "n") == 0 && (character >= 'A' && character <= 'Z'))
        {
            i--;
            continue;
        }

        // Escape lowercase letters
        if (strcmp(include_lowercase, "n") == 0 && (character >= 'a' && character <= 'z'))
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

        generated_password[i] = character;
    }

    generated_password[*length] = '\0';

    is_password_generated = 1;

    printf("\n\t\tGenerated password: %s\n", generated_password);

    free_all(5, length, include_uppercase, include_lowercase, numbers, special_chars);

    return 0;
}

int add_account_data(sqlite3 *db)
{
    Account *account = (Account *)malloc(sizeof(Account));

    account->site = get_string("\n\tSite: ", MAX_LENGTH);
    account->username = get_string("\tUsername: ", MAX_LENGTH);

    char *entered_password = get_string("\tPassword (enter * to use generated one): ", MAX_LENGTH);

    if (strcmp(entered_password, "*") == 0)
    {
        if (generated_password == NULL)
        {
            printf("\n\t\tNo password generated. Account is not saved");

            free_account(account);
            free(entered_password);

            return 1;
        }
        else
        {
            account->password = generated_password;

            // The generated password has been used, a new one and no freeing is needed
            is_password_generated = 0;
        }
    }
    else
    {
        account->password = entered_password;
    }

    account->user_id = atoi(getenv("SESSION_ID"));

    save_account(db, account);

    free_account(account);

    // The entered password has already been freed if it has been assigned to an account
    if (strcmp(entered_password, "*") == 0)
    {
        free(entered_password);
    }

    return 0;
}

int delete_account(sqlite3 *db)
{
    int *id = get_int("\n\tAccount id: ");

    delete_account_by_id(db, *id);

    free(id);

    return 0;
}

int edit_account(sqlite3 *db)
{
    int *id = get_int("\n\tAccount id: ");

    Account *account = get_account_by_id(db, *id);

    if (account == NULL)
    {
        free(id);
        return 1;
    }

    // Free for the new data
    free(account->username);
    free(account->password);

    account->username = get_string("\tUsername: ", MAX_LENGTH);
    char *entered_password = get_string("\tPassword (enter * to use generated): ", MAX_LENGTH);

    if (strcmp(entered_password, "*") == 0)
    {
        if (generated_password == NULL)
        {
            account->password = get_string("\tNo generated password. Enter another one: ", MAX_LENGTH);
        }
        else
        {
            account->password = generated_password;
        }

        // The entered password is not needed
        free(entered_password);
    }
    else
    {
        account->password = entered_password;
    }

    update_account(db, account);

    free_account(account);
    free(id);

    return 0;
}

int list_all_accounts(sqlite3 *db)
{
    int user_id = atoi(getenv("SESSION_ID"));
    int size;

    Account *user_accounts = get_all_accounts(db, user_id, &size);

    if (user_accounts == NULL)
    {
        return 1;
    }

    print_accounts(user_accounts, size);

    free(user_accounts);

    return 0;
}

int login(sqlite3 *db)
{
    char *username = get_string("\n\tUsername: ", MAX_LENGTH);
    char *master_password = get_password("\tMaster password: ", MAX_LENGTH);

    unsigned char *password_from_db = get_users_password(db, username);

    if (password_from_db == NULL)
    {
        printf("\n\t\tIncorrect username\n");

        free_all(3, username, master_password, password_from_db);

        return 1;
    }

    unsigned char *hash = malloc(EVP_MAX_MD_SIZE);
    unsigned int length = 0;

    hash_password(master_password, hash, &length);

    // Compare passwords
    if (memcmp(password_from_db, hash, strlen((char *)password_from_db)) != 0)
    {
        printf("\n\t\tIncorrect master password\n");

        free_all(4, username, master_password, password_from_db, hash);

        return 1;
    }

    const int user_id = get_users_id(db, username);

    // Convert user_id to a string
    char user[3];
    sprintf(user, "%d", user_id);

    // Track session
    setenv("SESSION_ID", user, 1);

    printf("\n\t\tLogin successfull\n");

    free_all(4, username, master_password, password_from_db, hash);

    return 0;
}

int register_user(sqlite3 *db)
{
    User *user = (User *)malloc(sizeof(User));

    user->first_name = get_string("\n\tFirst name: ", MAX_LENGTH);
    user->last_name = get_string("\tLast name: ", MAX_LENGTH);
    user->username = get_string("\tUsername: ", MAX_LENGTH);
    char *master_password = get_password("\tMaster password: ", MAX_LENGTH);

    unsigned char *hash = calloc(EVP_MAX_MD_SIZE + 1, sizeof(char));
    unsigned int length = 0;

    hash_password(master_password, hash, &length);

    user->password = hash;

    int success = save_user(db, user);

    free(master_password);
    free_user(user);

    return success;
}
