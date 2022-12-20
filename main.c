#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

int choose_option(void);
int register_user();

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

    // Ask user to choose the option
    int option = choose_option();

    if (option == 2)
    {
        register_user(db);
    }

    sqlite3_close(db);
    return 0;
}

int choose_option(void)
{
    int option = 0;

    while (option != 1 && option != 2)
    {
        printf("1. Login \n");
        printf("2. Register \n");
        printf("Choose an option (1/2): ");

        scanf("%d", &option);

        // Flush the buffer
        char line[100];
        fgets(line, sizeof(line), stdin);
    }

    return option;
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

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strlen(password) - 1] = '\0';

    // Validate input
    if (strlen(first_name) == 0 || strlen(last_name) == 0 ||
        strlen(username) == 0 || strlen(password) == 0)
    {
        fprintf(stderr, "Error: All fields are required\n");
        return -1;
    }

    // Construct the INSERT statement
    char *insert_query = sqlite3_mprintf(
        "INSERT INTO users (first_name, last_name, username, password) "
        "VALUES ('%q', '%q', '%q', '%q');",
        first_name, last_name, username, password);

    // Execute the INSERT statement
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, insert_query, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_free(insert_query);
        return -1;
    }

    sqlite3_free(insert_query);
    return 0;
}
