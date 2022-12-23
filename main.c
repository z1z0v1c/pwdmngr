#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

int choose_login_register_option(void);
int choose_site_data_option(void);
int login(sqlite3 *db);
int register_user(sqlite3 *db);

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
    int next_option = choose_site_data_option();

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

    char *err_msg = 0;
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT user_id, password FROM users WHERE username = ?", -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_text(res, 1, username, -1, NULL);
    }
    else
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    int step = sqlite3_step(res);
    const int user_id = sqlite3_column_int(res, 0);
    const char *text = sqlite3_column_text(res, 1);

    if (step != SQLITE_ROW || strcmp(text, password) != 0)
    {
        printf("\nUsername or password are incorrect\n");
        sqlite3_finalize(res);
        return 0;
    }

    // Convert the integer to a string and store the result in the str buffer
    char str[3];
    sprintf(str, "%d", user_id);

    // Track session
    setenv("SESSION_ID", str, 1);

    printf("\nLogin successfull\n");
    sqlite3_finalize(res);
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

    printf("\nRegister successfull\n");
    sqlite3_free(insert_query);
    return 0;
}
