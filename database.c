#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "database.h"

Account *get_all_accounts(sqlite3 *db, int user_id, int *size)
{
    int rc;
    int count = 0;
    *size = 2;
    Account *accounts = malloc(sizeof(Account) * *size);
    sqlite3_stmt *res = NULL;

    rc = sqlite3_prepare_v2(db, "SELECT * FROM accounts WHERE user_id = ?", -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        free(accounts);
        return NULL;
    }

    sqlite3_bind_int(res, 1, user_id);

    while ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        // reallocate the accounts array if needed
        if (count == *size)
        {
            *size *= 2;
            accounts = realloc(accounts, sizeof(Account) * *size);
            if (accounts == NULL)
            {
                fprintf(stderr, "Error allocating memory for accounts\n");
                sqlite3_finalize(res);
                return NULL;
            }
        }

        // process the values in the current row
        accounts[count].id = sqlite3_column_int(res, 0);
        accounts[count].user_id = sqlite3_column_int(res, 1);
        accounts[count].site = malloc(sqlite3_column_bytes(res, 2) + 1);
        strcpy(accounts[count].site, (char *)sqlite3_column_text(res, 2));
        accounts[count].username = malloc(sqlite3_column_bytes(res, 3) + 1);
        strcpy(accounts[count].username, (char *)sqlite3_column_text(res, 3));
        accounts[count].password = malloc(sqlite3_column_bytes(res, 4) + 1);
        strcpy(accounts[count].password, (char *)sqlite3_column_text(res, 4));

        count++;
    }
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Error stepping statement: %s\n", sqlite3_errmsg(db));
        free(accounts);
        sqlite3_finalize(res);
        return NULL;
    }

    // check if no accounts were found
    if (count == 0)
    {
        fprintf(stderr, "No accounts found for user ID %d\n", user_id);
        free(accounts);
        accounts = NULL;
    }
    else
    {
        // shrink the array to the actual number of accounts
        accounts = realloc(accounts, sizeof(Account) * count);
    }

    sqlite3_finalize(res);
    return accounts;
}

char *get_users_password(sqlite3 *db, char *username)
{
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT password FROM users WHERE username = ?", -1, &res, 0);

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
        char *password = "wrong password";
        return password;
    }

    char *password = (char *)sqlite3_column_text(res, 0);
    int password_length = strlen(password);

    char *result = (char *)malloc(password_length + 1);
    strncpy(result, password, password_length);
    result[password_length] = '\0';

    sqlite3_finalize(res);
    return result;
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

int save_account(sqlite3 *db, Account *account)
{
    // Construct the INSERT statement
    char *insert_query = sqlite3_mprintf(
        "INSERT INTO accounts (user_id, site, username, password) "
        "VALUES ('%d', '%q', '%q', '%q');",
        account->user_id, account->site, account->username, account->password);

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

int save_user(sqlite3 *db, User *user)
{
    // Construct the INSERT statement
    char *insert_query = sqlite3_mprintf(
        "INSERT INTO users (first_name, last_name, username, password) "
        "VALUES ('%q', '%q', '%q', '%q');",
        user->first_name, user->last_name, user->username, user->password);

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
