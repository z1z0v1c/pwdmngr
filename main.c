#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "database.h"

int choose_login_register_option(void);
int choose_site_data_option(void);

int login(sqlite3 *db);
int register_user(sqlite3 *db);
int add_account_data(sqlite3 *db);

int main(void)
{
    // Open database
    sqlite3 *db;
    int rc = sqlite3_open("pwdmngr.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return (0);
    }

    printf("Choose options by specifying a number\n");

    // Ask user to choose the option
    int option = choose_login_register_option();

    if (option == 1)
    {
        login(db);
    }
    else
    {
        register_user(db);
    }

    // Ask user to choose the option
    option = choose_site_data_option();

    switch (option)
    {
    case 2:
        add_account_data(db);
        break;

    default:
        break;
    }

    sqlite3_close(db);
    return 0;
}

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

int choose_site_data_option(void)
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
    char username[30];
    char password[30];

    // Prompt the user for the username, and password
    printf("\n\tUsername: ");
    fgets(username, sizeof(username), stdin);
    username[strlen(username) - 1] = '\0';

    printf("\tMaster password: ");
    fgets(password, sizeof(password), stdin);
    password[strlen(password) - 1] = '\0';

    // Validate input
    if (strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    char *db_password = get_users_password(db, username);

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
    return 0;
}

int register_user(sqlite3 *db)
{
    char first_name[30];
    char last_name[30];
    char username[30];
    char password[30];

    // Prompt the user for their first name, last name, username, and password
    printf("\nFirst name: ");
    fgets(first_name, sizeof(first_name), stdin);
    first_name[strlen(first_name) - 1] = '\0';

    printf("Last name: ");
    fgets(last_name, sizeof(last_name), stdin);
    last_name[strlen(last_name) - 1] = '\0';

    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strlen(username) - 1] = '\0';

    printf("Master password: ");
    fgets(password, sizeof(password), stdin);
    password[strlen(password) - 1] = '\0';

    // Validate input
    if (strlen(first_name) == 0 || strlen(last_name) == 0 ||
        strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    save_user(db, first_name, last_name, username, password);

    printf("\nRegister successfull\n");
    
    return 0;
}

int add_account_data(sqlite3 *db)
{
    printf("\n\tSite: ");
    char site[100];
    fgets(site, sizeof(site), stdin);
    site[strcspn(site, "\n")] = 0;

    printf("\tUsername: ");
    char username[100];
    fgets(username, sizeof(username), stdin);
    site[strcspn(username, "\n")] = 0;

    printf("\tPassword: ");
    char password[100];
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    // Validate input
    if (strlen(site) == 0 || strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    const char *user_id = getenv("SESSION_ID");

    save_account(db, user_id, site, username, password);

    printf("\nSuccessfull\n");
    return 0;
}
