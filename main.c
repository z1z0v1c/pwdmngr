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

    if (rc)
    {
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
        return 1;
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
        fprintf(stderr, "Invalid option\n");
        sqlite3_close(db);
        return 1;
    }

    // main loop
    int running = 1;
    while (running)
    {
        // Ask user to choose the option
        option = choose_account_data_option();

        switch (option)
        {
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

    sqlite3_close(db);
    return 0;
}
