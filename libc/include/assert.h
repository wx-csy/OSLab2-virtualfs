// Since NDEBUG affects the behavior of this header,
// include guard should not be used here.

#include <stdio.h>
#include <stdlib.h>

#ifdef assert
# undef assert
#endif

#ifdef NDEBUG
# define assert(ignore) ((void) 0)
#else
# define assert(cond) \
  do { \
    if (!(cond)) { \
      printf("%s:%d: Assertion `%s' failed.\n", \
          __FILE__, __LINE__, #cond); \
      abort(); \
    } \
  } while (0)
#endif

