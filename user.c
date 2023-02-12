#include <stdlib.h>
#include "user.h"

void free_user(User *user)
{
    free(user->first_name);
    free(user->last_name);
    free(user->username);
    free(user->password);
    free(user);
}
