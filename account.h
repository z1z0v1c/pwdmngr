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
