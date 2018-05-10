#include <stdio.h>
#include <am.h>

int puts(const char *str) {
  while (*str) _putc(*(str++));
  _putc('\n');
  return 1;
}

