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

void testshell(void *igonre) {
  
  test_mttest();
  test_mttest2();
  test_procfs(); 
  test_devfs();
  test_kvfs();
  test_concrw();

  _Exit(0);
}

