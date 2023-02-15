#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "database.h"

int delete_account_by_id(sqlite3 *db, int id)
{
    int user_id = atoi(getenv("SESSION_ID"));

    // Construct DELETE statement
    char *delete_query = sqlite3_mprintf("DELETE FROM accounts WHERE user_id = %d and id = %d;", user_id, id);

    // Execute DELETE statement
    char *error_message = 0;
    int rc = sqlite3_exec(db, delete_query, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_free(delete_query);
        return -1;
    }

    // Free memory
    sqlite3_free(delete_query);

    return 0;
}

Account *get_account_by_id(sqlite3 *db, int id)
{
    int rc;
    Account *account = malloc(sizeof(Account));
    sqlite3_stmt *res = NULL;

    // Construct the SELECT query
    rc = sqlite3_prepare_v2(db, "SELECT * FROM accounts WHERE id = ? AND user_id = ?", -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        free(account);
        return NULL;
    }

    sqlite3_bind_int(res, 1, id);

    int user_id = atoi(getenv("SESSION_ID"));
    sqlite3_bind_int(res, 2, user_id);

    if ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        // Process the values in row
        account->id = sqlite3_column_int(res, 0);
        account->user_id = sqlite3_column_int(res, 1);
        account->site = malloc(sqlite3_column_bytes(res, 2) + 1);
        strcpy(account->site, (char *)sqlite3_column_text(res, 2));
        account->username = malloc(sqlite3_column_bytes(res, 3) + 1);
        strcpy(account->username, (char *)sqlite3_column_text(res, 3));
        account->password = malloc(sqlite3_column_bytes(res, 4) + 1);
        strcpy(account->password, (char *)sqlite3_column_text(res, 4));
    }
    else
    {
        fprintf(stderr, "\n\t\tNo accounts found for id: %d\n", id);

        // Free memory
        sqlite3_finalize(res);
        free(account);

        return NULL;
    }

    // Free memory
    sqlite3_finalize(res);

    return account;
}

Account *get_all_accounts(sqlite3 *db, int user_id, int *size)
{
    int rc;
    int count = 0;
    *size = 2;
    Account *accounts = malloc(sizeof(Account) * *size);
    sqlite3_stmt *res = NULL;

    // Construct the SELECT query
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
        // Reallocate the accounts array if needed
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

        // Process the values in the current row
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

    // Check if no accounts were found
    if (count == 0)
    {
        fprintf(stderr, "No accounts found for user ID %d\n", user_id);
        free(accounts);
        accounts = NULL;
    }
    else
    {
        // Shrink the array to the actual number of accounts
        accounts = realloc(accounts, sizeof(Account) * count);
    }

    *size = count;

    // Free memory
    sqlite3_finalize(res);

    return accounts;
}

unsigned char *get_users_password(sqlite3 *db, char *username)
{
    // Construct the SELECT query
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT password FROM users WHERE username = ?", -1, &res, 0);

    // Validate query
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
        exit(1);
    }

    // Get data
    unsigned char *password = (unsigned char *)sqlite3_column_text(res, 0);
    int password_length = strlen((char *)password);

    unsigned char *result = (unsigned char *)malloc(password_length + 1);
    memcpy(result, password, password_length);
    result[password_length] = '\0';

    // Free memory
    sqlite3_finalize(res);

    return result;
}

int get_users_id(sqlite3 *db, char *username)
{
    // Construct the SELECT query
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT user_id FROM users WHERE username = ?", -1, &res, 0);

    // Validate query
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

    // Get data
    int user_id = sqlite3_column_int(res, 0);

    // Free memory
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
    char *error_message = 0;
    int rc = sqlite3_exec(db, insert_query, 0, 0, &error_message);

    // Check success
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_free(insert_query);
        return -1;
    }
    else
    {
        printf("\n\t\tAccount saved successfully\n");
    }

    // Free memory
    sqlite3_free(insert_query);

    return 0;
}

int save_user(sqlite3 *db, User *user)
{
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (first_name, last_name, username, password) VALUES (?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, user->first_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, user->last_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, user->username, -1, SQLITE_STATIC);
#ifdef OPENSSL_EVP_H
        sqlite3_bind_blob(stmt, 4, user->password, strlen((char *)user->password), SQLITE_STATIC);
#else
        sqlite3_bind_text(stmt, 3, user->password, -1, SQLITE_STATIC);
#endif

        int result = sqlite3_step(stmt);

        // Handle error
        if (result != SQLITE_DONE)
        {
            if (result == SQLITE_CONSTRAINT)
            {
                printf("\n\t\tUsername %s already exists. Try again.\n", user->username);
            }
            else
            {
                printf("\n\t\t%s\n", "Something went wrong.");
            }

            // Free memory
            sqlite3_finalize(stmt);

            return 1;
        }
    }

    printf("\n\t\tRegistered successfully\n");

    // Free memory
    sqlite3_finalize(stmt);

    return 0;
}

int update_account(sqlite3 *db, Account *account)
{
    // Construct the INSERT statement
    char *update_query = sqlite3_mprintf(
        "UPDATE accounts SET username = '%q', password = '%q' WHERE id = '%d'",
        account->username, account->password, account->id);

    // Execute the INSERT statement
    char *error_message = 0;
    int rc = sqlite3_exec(db, update_query, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_free(update_query);
        return -1;
    }

    // Free memory
    sqlite3_free(update_query);

    return 0;
}
