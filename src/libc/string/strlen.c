#include <string.h>

size_t strlen(const char *str) {
  const char *ptr = str;
  while (*ptr) ptr++;
  return ptr - str;
}

