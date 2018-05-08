#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>

#define panic(fmt, ...) \
  do { \
    printf("%s:%d: fatal error occurred!\n", \
        __FILE__, __LINE__); \
    printf(fmt, ##__VA_ARGS__); \
    abort(); \
  } while (0)
#endif

