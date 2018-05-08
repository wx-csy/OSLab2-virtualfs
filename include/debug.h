#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG_ME
# define $debug(fmt, ...) printf("\033[94m%s: " fmt "\n\033[0m", \
    __func__, ##__VA_ARGS__) 
#else 
# define $debug(...) ((void) 0)
#endif

#endif
