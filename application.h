#include <sqlite3.h>

int choose_login_register_option(void);
int choose_account_data_option(void);
int login(sqlite3 *db);
int register_user(sqlite3 *db);
int add_account_data(sqlite3 *db);