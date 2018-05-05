#ifndef _cstring_h
#define _cstring_h

#include <stddef.h>

#ifndef NULL
# define NULL 0
#endif

void *memset(void *dest, int ch, size_t count);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
size_t strlen(const char *str);
char *strcpy(char *restrict dest, const char *restrict src);

#endif

