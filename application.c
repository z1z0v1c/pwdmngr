#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "application.h"
#include "database.h"
#include "helper.h"

int choose_login_register_option(void)
{
    int option = 0;

    while (option != 1 && option != 2)
    {
        printf("\n1. Login \n");
        printf("2. Register \n");
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

    while (option != 1 && option != 2 && option != 3 && option != 4 && option != 5)
    {
        printf("\n1. Generate password \n");
        printf("2. Add account\n");
        printf("3. Edit account\n");
        printf("4. Delete account\n");
        printf("5. List all accounts\n");
        printf("\nChoose an option : ");

        scanf("%d", &option);

        // Flush the buffer
        char line[100];
        fgets(line, sizeof(line), stdin);
    }

    return option;
}

int login(sqlite3 *db)
{
    char *username = get_string("\n\tUsername: ", 30);
    char *password = get_string("\tMaster password: ", 30);

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
        return 0;
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
    User *user =(User *)malloc(sizeof(User));
    user->first_name = get_string("\nFirst name: ", 30);
    user->last_name = get_string("Last name: ", 30);
    user->username = get_string("Username: ", 30);
    user->password = get_string("Master password: ", 30);

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
    account->site = get_string("\n\tSite: ", 30);
    account->username = get_string("\tUsername: ", 30);
    account->password = get_string("\tpassword: ", 30);

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

int list_all_accounts(sqlite3 *db)
{
    return 0;
}