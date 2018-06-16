#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>

#define VERDICT(result, fmt, ...) \
  do { \
    if ((result) == 0) printf("\033[92mOK \033[0m"); \
    else printf("\033[91mFAIL \033[0m"); \
    printf(fmt "\n", ##__VA_ARGS__); \
  } while (0) 


void init_test();

int sh_create_thread(thread_t *thread, void entry(void *arg), void *arg);
void sh_teardown(thread_t *thread);

int test_mttest();

#endif

