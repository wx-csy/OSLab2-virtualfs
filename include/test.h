#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>

#define SLEEP(n) do { \
    for (volatile int i = 0; i < n * 1000000; i++); \
  } while (0)

#define VERDICT(result, fmt, ...) \
  do { \
    if ((result) == 0) printf("\033[92mOK \033[0m"); \
    else printf("\033[91mFAIL \033[0m"); \
    printf(fmt "\n\n\n", ##__VA_ARGS__); \
    SLEEP(200); \
    return result; \
  } while (0) 


void init_test();

int sh_create_thread(thread_t *thread, void entry(void *arg), void *arg);
void sh_teardown(thread_t *thread);

int shb_type(const char *path);

int test_mttest();
int test_mttest2();
int test_procfstest();

#endif

