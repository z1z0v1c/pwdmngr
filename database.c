#include <stdio.h>
#include "database.h"

char *get_users_password(sqlite3 *db, char *username)
{
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

    if (step != SQLITE_ROW)
    {
        printf("\nUsername are incorrect\n");
        sqlite3_finalize(res);
        return "wrong password";
    }

    char *password = (char*)sqlite3_column_text(res, 0);

    sqlite3_finalize(res);
    return password;
}

int get_users_id(sqlite3 *db, char *username)
{
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT user_id FROM users WHERE username = ?", -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_text(res, 1, username, -1, NULL);
    }
    else
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    int step = sqlite3_step(res);

    if (step != SQLITE_ROW)
    {
        printf("\nUsername are incorrect\n");
        sqlite3_finalize(res);
        return -1;
    }

    int user_id = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    return user_id;
}

int save_account(sqlite3 *db, int user_id, char *site, char *username, char *password)
{
    // Construct the INSERT statement
    char *insert_query = sqlite3_mprintf(
        "INSERT INTO accounts (user_id, site, username, password) "
        "VALUES ('%d', '%q', '%q', '%q');",
        user_id, site, username, password);

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

    // Free memory
    sqlite3_free(insert_query);

    return 0;
}

int save_user(sqlite3 *db, char *first_name, char *last_name, char *username, char *password)
{
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

    // Free memory
    sqlite3_free(insert_query);

    return 0;
}