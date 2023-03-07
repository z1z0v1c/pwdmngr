#ifndef HEADER_FILE_HELPER_H
#define HEADER_FILE_HELPER_H

#include "account.h"

// Free memory for all args 
void free_all(int count, ...);

int *get_int(char *tip);

// Get password from the standard input masked with asterisks
char *get_password(char *tip, int len);

char *get_string(char* tip, int length);

// Compute hash and store it to the "hash" buffer
void hash_password(const char *password, unsigned char *hash, unsigned int *length);

// Formatted printing of all users accounts
void print_accounts(Account *user_accounts, int size);

#endif
