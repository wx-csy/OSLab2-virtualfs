#ifndef _stdio_h
#define _stdio_h

#include <stdarg.h>

int puts(const char *str);

int printf(const char *restrict format, ...);
int sprintf(char *restrict buffer, const char *restrict format, ...);

// helper function
int _vprintf_raw(void (*putc)(char), const char *restrict format, 
    va_list args);

#endif

