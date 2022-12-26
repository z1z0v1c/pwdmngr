#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "database.h"
#include "helper.h"

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

    switch (option)
    {
    case 2:
        register_user(db);
    case 1:
        login(db);
        break;

    default:
        break;
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
    char* username = get_string("\n\tUsername: ", 30);
    char* password = get_string("\tMaster password: ", 30);

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
    char* first_name = get_string("\nFirst name: ", 30);
    char* last_name = get_string("Last name: ", 30);
    char* username = get_string("Username: ", 30);
    char* password = get_string("Master password: ", 30);

    // Validate input
    if (strlen(first_name) == 0 || strlen(last_name) == 0 ||
        strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    save_user(db, first_name, last_name, username, password);

    printf("\nRegister successfull\n");
    
    free_all(4, first_name, last_name, username, password);
    return 0;
}

int add_account_data(sqlite3 *db)
{
    char* site = get_string("\n\tSite: ", 30);
    char* username = get_string("\tUsername: ", 30);
    char* password = get_string("\tpassword: ", 30);

    // Validate input
    if (strlen(site) == 0 || strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    int user_id = atoi(getenv("SESSION_ID"));

    save_account(db, user_id, site, username, password);

    printf("\nSuccessfull\n");

    free_all(3, site, username, password);
    return 0;
}
