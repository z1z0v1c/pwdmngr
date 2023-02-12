#ifndef HEADER_FILE_USER_H
#define HEADER_FILE_USER_H

typedef struct
{
    char *first_name;
    char *last_name;
    char *username;
    char *password;
} User;

// Free memory for an user
void free_user(User *user);

#endif
