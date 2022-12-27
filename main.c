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
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
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

    while (1)
    {
        // Ask user to choose the option
        option = choose_account_data_option();

        switch (option)
        {
        case 2:
            add_account_data(db);
            break;
        case 5:
            list_all_accounts(db);
            break;

        default:
            break;
        }

        char *should_continue = get_string("\nDo you want to continue (y/n): ", 3);

        if (strcmp("y", should_continue) != 0 && strcmp("yes", should_continue) != 0)
        {
            break;
        }

        free(should_continue);
    }

    sqlite3_close(db);
    return 0;
}
