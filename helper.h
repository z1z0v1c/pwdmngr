#include "account.h"

void free_all(int count, ...);
void free_user(User *user);
void free_account(Account *account);
int *get_int(char *tip);
char *get_password(char *tip, int len);
char* get_string(char* tip, int length);
