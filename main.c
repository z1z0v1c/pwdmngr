#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "application.h"
#include "database.h"
#include "helper.h"

int main(void)
{
    // Open database
    sqlite3 *db;
    int rc = sqlite3_open("pwdmngr.db", &db);

    // Exit if database can't be opened
    if (rc)
    {
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Print initial message
    printf("\nCHOOSE OPTIONS BY SPECIFYING A NUMBER\n");

    int running = 1;
    int *option = NULL;

    // Login loop
    while (running)
    {
        // Print options
        printf("\n1. Register \n");
        printf("2. Login \n");
        printf("3. Exit \n");

        // Free memory for the new iteration
        if (option != NULL)
        {
            free(option);
        }

        // Get an option
        option = get_int("\nChoose an option: "); 

        switch (*option)
        {
        case 1:
            register_user(db);
            break;
        case 2:
            running = login(db);
            break;
        case 3:
            sqlite3_close(db);
            return 0;
        default:
            printf("\n\tInvalid option\n");
            break;
        }
    }

    // Prepare for the main loop
    running = 1;

    // Main loop
    while (running)
    {
        // Print options
        printf("\n1. Generate password \n");
        printf("2. Add account\n");
        printf("3. Edit account\n");
        printf("4. Delete account\n");
        printf("5. List all accounts\n");
        printf("6. Exit\n");

        // Free memory for the new iteration
        if (option != NULL)
        {
            free(option);
        }

        // Get an option
        option = get_int("\nChoose an option: "); 

        switch (*option)
        {
        case 1:
            free_password();
            generate_password();
            break;
        case 2:
            add_account_data(db);
            break;
        case 3:
            edit_account(db);
            break;
        case 4:
            delete_account(db);
            break;
        case 5:
            list_all_accounts(db);
            break;
        case 6:
            running = 0;
            break;
        default:
            printf("\n\tInvalid option\n");
            break;
        }
    }

    // Free memory
    sqlite3_close(db);
    free_password();
    free(option);

    return 0;
}
