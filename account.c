#include <stdlib.h>
#include "account.h"

void free_account(Account *account)
{
    free(account->site);
    free(account->username);
    free(account->password);
    free(account);
}
