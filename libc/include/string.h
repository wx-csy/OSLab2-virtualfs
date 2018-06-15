#ifndef _cstring_h
#define _cstring_h

#include <stddef.h>
#include <stdint.h>

#ifndef NULL
# define NULL 0
#endif

void *memset(void *dest, int ch, size_t count);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
size_t strlen(const char *str);
char *strcpy(char *restrict dest, const char *restrict src);
char *strncpy(char *restrict dest, const char *restrict src, 
    size_t count);
int strcmp(const char *lhs, const char *rhs);
char *strtok(char *restrict str, const char *restrict delim);

#endif

