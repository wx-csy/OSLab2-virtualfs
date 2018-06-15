#include <string.h>
int strcmp(const char *lhs, const char *rhs) {
  while (*lhs && *rhs) {
    if (*lhs == *rhs) {
      lhs++; rhs++;
    } else {
      return (int)*lhs - (int)*rhs;
    }
  }
  return (int)*lhs - (int)*rhs;
}

