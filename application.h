#ifndef HEADER_FILE_APPLICATION_H
#define HEADER_FILE_APPLICATION_H

#include <sqlite3.h>

int generate_password(void);
void free_password(void);
int add_account_data(sqlite3 *db);
int delete_account(sqlite3 *db);
int edit_account(sqlite3 *db);
int list_all_accounts(sqlite3 *db);
int login(sqlite3 *db);
int register_user(sqlite3 *db);

#endif
