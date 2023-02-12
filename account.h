#ifndef HEADER_FILE_ACCOUNT_H
#define HEADER_FILE_ACCOUNT_H

typedef struct
{
    int id;
    int user_id;
    char *site;
    char *username;
    char *password;
} Account;

// Free memory for an account
void free_account(Account *account);

#endif
