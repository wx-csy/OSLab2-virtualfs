#include <string.h>
#include <stdio.h>

char *strncpy(char *restrict dest, const char *restrict src, 
    size_t count) {
  char *ret = dest;
  while (dest - ret < count) {
    *(dest++) = *(src);
    if (*src) src++;
  }
  return ret;
}

