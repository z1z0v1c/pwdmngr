#include <sqlite3.h>

int choose_login_register_option(void);
int choose_account_data_option(void);
int generate_password();
void free_password();
int add_account_data(sqlite3 *db);
int delete_account(sqlite3 *db);
int edit_account(sqlite3 *db);
int list_all_accounts(sqlite3 *db);
int login(sqlite3 *db);
int register_user(sqlite3 *db);
