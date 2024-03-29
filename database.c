#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "database.h"

int delete_account_by_id(sqlite3 *db, int id)
{
    int user_id = atoi(getenv("SESSION_ID"));

    // Construct the DELETE statement
    char *delete_stmt = sqlite3_mprintf("DELETE FROM accounts WHERE user_id = %d and id = %d;", user_id, id);

    // Execute the DELETE statement
    char *error_message = 0;
    int rc = sqlite3_exec(db, delete_stmt, 0, 0, &error_message);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", error_message);

        sqlite3_free(error_message);
        sqlite3_free(delete_stmt);

        return -1;
    }

    sqlite3_free(delete_stmt);

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

    int user_id = atoi(getenv("SESSION_ID"));

    // Bind parameters
    sqlite3_bind_int(res, 1, id);
    sqlite3_bind_int(res, 2, user_id);

    // If the SELECT statement has a result
    if ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        // Process the values in the row
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
        fprintf(stderr, "\n\t\tAccount with Id = %d hasn't been found.\n", id);

        sqlite3_finalize(res);
        free(account);

        return NULL;
    }

    sqlite3_finalize(res);

    return account;
}

Account *get_all_accounts(sqlite3 *db, int user_id, int *size)
{
    // Set initial size for the list of accounts 
    *size = 2;
    
    int rc;
    int count = 0;
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

    // Bind parameter
    sqlite3_bind_int(res, 1, user_id);

    // If the SELECT statement has results
    while ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        // Reallocate the accounts array if needed
        if (count == *size)
        {
            *size *= 2;
            accounts = realloc(accounts, sizeof(Account) * *size);

            if (accounts == NULL)
            {
                fprintf(stderr, "Error allocating memory for accounts.\n");

                sqlite3_finalize(res);

                return NULL;
            }
        }

        // Process the values in a row
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

    if (count == 0)
    {
        fprintf(stderr, "\n\tNo accounts have been found.\n");

        free(accounts);
        accounts = NULL;
    }
    else
    {
        // Shrink the array to the actual number of accounts
        accounts = realloc(accounts, sizeof(Account) * count);
    }

    // Set final size
    *size = count;

    sqlite3_finalize(res);

    return accounts;
}

unsigned char *get_users_password(sqlite3 *db, char *username)
{
    // Construct the SELECT query
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT password FROM users WHERE username = ?", -1, &res, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(res);

        return NULL;
    }

    // Bind parameter
    sqlite3_bind_text(res, 1, username, -1, NULL);

    if (sqlite3_step(res) != SQLITE_ROW)
    {
        sqlite3_finalize(res);

        return NULL;
    }

    // Get the password from the result
    unsigned char *password = (unsigned char *)sqlite3_column_text(res, 0);
    int password_length = strlen((char *)password);

    unsigned char *final_password = (unsigned char *)malloc(password_length + 1);
    memcpy(final_password, password, password_length);
    final_password[password_length] = '\0';

    sqlite3_finalize(res);

    return final_password;
}

int get_users_id(sqlite3 *db, char *username)
{
    // Construct the SELECT query
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db, "SELECT user_id FROM users WHERE username = ?", -1, &res, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(res);

        return -1;
    }

    // Bind parameter
    sqlite3_bind_text(res, 1, username, -1, NULL);

    if (sqlite3_step(res) != SQLITE_ROW)
    {
        printf("\nIncorrect username!\n");

        sqlite3_finalize(res);

        return -1;
    }

    // Get the users id from the result
    int user_id = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);

    return user_id;
}

int save_account(sqlite3 *db, Account *account)
{
    // Construct the INSERT statement
    char *insert_stmt = sqlite3_mprintf(
        "INSERT INTO accounts (user_id, site, username, password) "
        "VALUES ('%d', '%q', '%q', '%q');",
        account->user_id, account->site, account->username, account->password);

    // Execute the INSERT statement
    char *error_message = 0;
    int rc = sqlite3_exec(db, insert_stmt, 0, 0, &error_message);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", error_message);

        sqlite3_free(error_message);
        sqlite3_free(insert_stmt);

        return 1;
    }

    printf("\n\t\tAccount saved successfully!\n");

    sqlite3_free(insert_stmt);

    return 0;
}

int save_user(sqlite3 *db, User *user)
{
    // Construct the INSERT statement
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (first_name, last_name, username, password) VALUES (?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(stmt);

        return -1;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, user->first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user->last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 4, user->password, strlen((char *)user->password), SQLITE_STATIC);

    int result = sqlite3_step(stmt);

    if (result != SQLITE_DONE)
    {
        if (result == SQLITE_CONSTRAINT)
        {
            printf("\n\t\tUsername %s already exists! Try again!\n", user->username);
        }
        else
        {
            printf("\n\t\t%s\n", "Something went wrong.");
        }

        sqlite3_finalize(stmt);

        return 1;
    }

    printf("\n\t\tRegistered successfully!\n");

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

        return 1;
    }

    printf("\n\t\tAccount updated successfully!\n");

    sqlite3_free(update_query);

    return 0;
}
