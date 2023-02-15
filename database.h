#ifndef HEADER_FILE_DATABASE_H
#define HEADER_FILE_DATABASE_H

#include <sqlite3.h>
#include "account.h"
#include "user.h"

int delete_account_by_id(sqlite3 *db, int id);
Account *get_account_by_id(sqlite3 *db, int id);
Account *get_all_accounts(sqlite3 *db, int user_id, int *size);
unsigned char *get_users_password(sqlite3 *db, char *username);
int get_users_id(sqlite3 *db, char *username);
int save_user(sqlite3 *db, User *user);
int save_account(sqlite3 *db, Account *account);
int update_account(sqlite3 *db, Account *account);

#endif
