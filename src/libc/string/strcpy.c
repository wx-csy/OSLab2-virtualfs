#include <string.h>

char *strcpy(char *restrict dest, const char *restrict src) {
  char *ret = dest;
  do {
    *(dest++) = *(src++);
  } while (*src); 
  return ret;
}

