#include <sqlite3.h>

char* get_users_password(sqlite3* db, char* username);
char* get_users_id(sqlite3* db, char* username);
int save_user(sqlite3* db, char* first_name, char* last_name, char* username, char* password);
int save_account(sqlite3* db, int user_id, char* site, char* username, char* password);
