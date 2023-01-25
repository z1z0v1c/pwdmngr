#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include "application.h"
#include "database.h"
#include "helper.h"

#define MAX_LENGTH 30

int choose_login_register_option(void)
{
    int option = 0;

    while (option != 1 && option != 2)
    {
        printf("\n1. Register \n");
        printf("2. Login \n");
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

int choose_password_option(void)
{
    int option = 0;

    while (option != 1 && option != 2 && option != 3)
    {
        printf("\n\t1. Letters and numbers \n");
        printf("\t2. Letters and special characters \n");
        printf("\t3. All \n");
        printf("\n\tChoose an option : ");

        scanf("%d", &option);

        // Flush the buffer
        char line[100];
        fgets(line, sizeof(line), stdin);
    }

    return option;
}

void generate_letters_chars(int length, char *password)
{
    for (int i = 0; i < length; i++)
    {
        char character = (char)(rand() % (126 - 33 + 1) + 33);

        if (character > '/' && character < ':')
        {
            i--;
            continue;
        }

        password[i] = character;
    }

    password[length] = '\0';
}

void generate_letters_numbers(int length, char *password)
{
    for (int i = 0; i < length; i++)
    {
        char character = (char)(rand() % (122 - 48 + 1) + 48);

        if ((character > '9' && character < 'A') || (character > 'Z' && character < 'a'))
        {
            i--;
            continue;
        }

        password[i] = character;
    }

    password[length] = '\0';
}

int generate_password()
{
    int *length = get_int("\n\tSpecify length: "); 
    char *password = malloc(sizeof(char) * *length + 1);
    memset(password, 0, *length + 1);

    int option = choose_password_option();

    switch (option)
    {
    case 1:
        generate_letters_numbers(*length, password);
        break;
    case 2:
        generate_letters_chars(*length, password);
        break;
    
    default:
        *password = '\0';
        break;
    }

    printf("\n\t\tGenerated password: %s\n", password);

    free(length);
    free(password);

    return 0;
}

int login(sqlite3 *db)
{
    char *username = get_string("\n\tUsername: ", MAX_LENGTH);
    char *password = get_string("\tMaster password: ", MAX_LENGTH);

    // Validate input
    if (strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    // Get password for the user from database
    char *db_password = get_users_password(db, username);

    // Compare passwords
    if (strcmp(db_password, password) != 0)
    {
        printf("\nPassword are incorrect\n");
        exit(1);
    }

    const int user_id = get_users_id(db, username);

    // Convert the integer to a string and store the result in the str buffer
    char user[3];
    sprintf(user, "%d", user_id);

    // Track session
    setenv("SESSION_ID", user, 1);

    printf("\nLogin successfull\n");

    free_all(3, username, password, db_password);
    return 0;
}

int register_user(sqlite3 *db)
{
    User *user = (User *)malloc(sizeof(User));

    user->first_name = get_string("\nFirst name: ", MAX_LENGTH);
    user->last_name = get_string("Last name: ", MAX_LENGTH);
    user->username = get_string("Username: ", MAX_LENGTH);
    user->password = get_string("Master password: ", MAX_LENGTH);

    // Validate user
    if (strlen(user->first_name) == 0 || strlen(user->last_name) == 0 ||
        strlen(user->username) == 0 || strlen(user->password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    // Save user to db
    save_user(db, user);

    printf("\nRegister successfull\n");

    // Free user
    free_user(user);
    return 0;
}

int add_account_data(sqlite3 *db)
{
    Account *account = (Account *)malloc(sizeof(Account));

    account->site = get_string("\n\tSite: ", MAX_LENGTH);
    account->username = get_string("\tUsername: ", MAX_LENGTH);
    account->password = get_string("\tPassword: ", MAX_LENGTH);

    // Validate input
    if (strlen(account->site) == 0 || strlen(account->username) == 0 || strlen(account->password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    account->user_id = atoi(getenv("SESSION_ID"));

    save_account(db, account);

    printf("\nSuccessfull\n");

    free_account(account);
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

    free(account->username);
    free(account->password);

    account->username = get_string("\tUsername: ", MAX_LENGTH);
    account->password = get_string("\tPassword: ", MAX_LENGTH);

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
        return -1;
    }

    for (int i = 0; i < size; i++)
    {
        printf("\n\t%d. Id: %d | Site: %s | Username: %s | Password: %s", i + 1, user_accounts[i].id, user_accounts[i].site, user_accounts[i].username, user_accounts[i].password);
        free(user_accounts[i].site);
        free(user_accounts[i].username);
        free(user_accounts[i].password);
    }

    printf("%s", "\n"); 

    free(user_accounts);
    return 0;
}
