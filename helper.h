#ifndef HEADER_FILE_HELPER_H
#define HEADER_FILE_HELPER_H

// Free memory for all args 
void free_all(int count, ...);

// Get number from the standard input
int *get_int(char *tip);

// Get password from the standard input masked with asterisks
char *get_password(char *tip, int len);

// Get text from the standard input
char *get_string(char* tip, int length);

// Compute hash and store it to the "hash" buffer
void hash_password(const char *password, unsigned char *hash, unsigned int *length);

#endif
