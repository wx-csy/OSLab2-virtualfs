#include <stdlib.h>
#include <stdio.h>
#include <test.h>

struct testcase {
  const char *name;
  int (*fn)();
  int result;
} testcases[] = {
  {"Multi-threading Test (1)", test_mttest, 0},
  {"Multi-threading Test (2)", test_mttest2, 0},
  {"procfs Functional Test", test_procfs, 0},
  {"devfs Functional Test", test_devfs, 0},
  {"kvfs Functional Test", test_kvfs, 0},
  {"Concurrent R/W Test", test_concrw, 0}
};

#define NR_TESTCASES (sizeof(testcases) / sizeof(struct testcase))

void testshell() {
  init_test();

  for (int i = 0; i < NR_TESTCASES; i++) {
    testcases[i].result = testcases[i].fn();
  }
  
  SLEEP(200);

  int passcnt = 0;
  puts("========== TEST SUMMARY ===========");
  for (int i = 0; i < NR_TESTCASES; i++) {
    printf("  %s: ", testcases[i].name);
    if (testcases[i].result == 0) {
      passcnt++;
      puts("\033[92mOK\033[0m");
    } else {
      puts("\033[91mFAIL\033[0m");
    }
  }
  puts("");
  printf("%d of %d test cases passed", passcnt, NR_TESTCASES);
  _Exit(0);
}

