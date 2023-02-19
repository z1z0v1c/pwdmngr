#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "application.h"
#include "database.h"

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

    // Login loop
    while (running)
    {
        // Ask user to choose an option
        int option = choose_login_register_option();

        switch (option)
        {
        case 1:
            register_user(db);
            break;
        case 2:
            running = login(db);
            break;
        case 3:
            // Free memory
            sqlite3_close(db);
            return 0;
        default:
            fprintf(stderr, "Invalid option\n");
            break;
        }
    }

    // Main loop
    running = 1;
    while (running)
    {
        // Ask user to choose an option
        int option = choose_account_data_option();

        switch (option)
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
            fprintf(stderr, "Invalid option\n");
            break;
        }
    }

    // Free memory
    sqlite3_close(db);
    free_password();

    return 0;
}
