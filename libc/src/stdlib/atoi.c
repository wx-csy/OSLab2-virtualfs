#include <stdlib.h>
#include <ctype.h>

int atoi(const char *str) {
  while (*str && isspace(*str)) str++;
  if (*str == 0) return 0;
  unsigned ans = 0; int sign = 1;
  if (*str == '+') {
    sign = 1;
    str++;
  } else if (*str == '-') {
    sign = -1;
    str++;
  }
  while (isdigit(*str)) {
    ans = ans * 10 + (*str - '0');
    str++;
  }
  return ans * sign;
}

