#include <stdio.h>

static char *buf;

static void str_putc(char ch) {
  *(buf++) = ch;
}

int sprintf(char *restrict buffer, const char *restrict format, ...) {
  char *last_buf = buf;
  buf = buffer;
  va_list args;
  va_start(args, format);
  int ret = _vprintf_raw(&str_putc, format, args);
  *(buf++) = 0;
  va_end(args);
  buf = last_buf;
  return ret;
}

