#ifndef HEADER_FILE_HELPER_H
#define HEADER_FILE_HELPER_H

void free_all(int count, ...);
int *get_int(char *tip);
char *get_password(char *tip, int len);
char* get_string(char* tip, int length);
void hash_password(const char *password, unsigned char *hash, unsigned int *length);

#endif
