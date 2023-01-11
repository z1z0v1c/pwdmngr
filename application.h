#include <sqlite3.h>

typedef struct
{
    char *first_name;
    char *last_name;
    char *username;
    char *password;
} User;

typedef struct
{
    int id;
    int user_id;
    char *site;
    char *username;
    char *password;
} Account;

int choose_login_register_option(void);
int choose_account_data_option(void);
int login(sqlite3 *db);
int register_user(sqlite3 *db);
int add_account_data(sqlite3 *db);
int delete_account(sqlite3 *db);
int edit_account(sqlite3 *db);
int list_all_accounts(sqlite3 *db);
