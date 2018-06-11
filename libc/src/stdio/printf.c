#include <stdio.h>
#include <am.h>

int printf(const char *restrict format, ...) {
  va_list args;
  va_start(args, format);
  int ret = _vprintf_raw(&_putc, format, args);
  va_end(args);
  return ret;
}

