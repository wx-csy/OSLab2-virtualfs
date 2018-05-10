#include <string.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
  typedef unsigned char *restrict rpuchar;
  for (size_t i = 0; i < n; i++) 
    ((rpuchar)dest)[i] = ((rpuchar)src)[i];
  return dest;
}

