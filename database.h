#include <sqlite3.h>

Account *get_all_accounts(sqlite3 *db, int user_id, int *size);
char *get_users_password(sqlite3 *db, char *username);
int get_users_id(sqlite3 *db, char *username);
int save_user(sqlite3 *db, User *user);
int save_account(sqlite3 *db, Account *account);
