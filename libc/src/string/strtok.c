#include <string.h>

char *strtok(char *restrict str, const char *restrict delim) {
  static char *old = NULL;
  char delim_map[256] = {};
  while (*delim) {
    delim_map[(unsigned char)*delim] = 1;
    delim++;
  }
  if (str == NULL) str = old;
  if (str == NULL) return NULL;
  while (*str) {
    if (!delim_map[(unsigned char)*str]) goto cont;
    str++;
  }
  old = NULL;
  return NULL;
cont:
  old = str;
  while (*old) {
    if (delim_map[(unsigned char)*old]) {
      *old = 0;
      old++;
      return str;
    }
    old++;
  }
  old = NULL;
  return str;
}

