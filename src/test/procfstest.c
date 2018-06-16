#include <kernel.h>
#include <os.h>
#include <test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

int test_procfstest() {
  puts("====== procfs Functional Test (1) ======");
  puts("# test functions of procfs");
  int cnt = 0;
  if (shb_type("/proc/cpuinfo") == 0) cnt++;
  SLEEP(100);
  if (shb_type("/proc/meminfo") == 0) cnt++;
  SLEEP(100);
  char path[256];
  sprintf(path, "/proc/%d", this_thread->tid);
  if (shb_type(path) == 0) cnt++; 
  SLEEP(100);

  if (cnt == 3) {
    VERDICT(0, "%d files", cnt);
  } else {
    VERDICT(1, "%d of 3 files succeeded", cnt);
  }
}

