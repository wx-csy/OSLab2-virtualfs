#include <kernel.h>
#include <os.h>
#include <test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

int test_procfs() {
  puts("====== procfs Functional Test ======");
  puts("# test functions of procfs");
  int cnt = 0, chk_cnt = 0;
  
  // 4 checkpoints
  if (sh_access("/proc/cpuinfo", W_OK) < 0) chk_cnt++;
  int fd;
  int dummy;
  if ((fd = sh_open("/proc/cpuinfo", O_RDONLY)) >= 0) chk_cnt++;
  if (sh_write(fd, &dummy, sizeof dummy) < 0) chk_cnt++;
  if (sh_close(fd) == 0) chk_cnt++;

  SLEEP(100);
  if (sh_access("/proc/cpuinfo", R_OK) == 0 &&
      shb_type("/proc/cpuinfo") == 0) cnt++;
  SLEEP(100);
  if (sh_access("/proc/meminfo", R_OK) == 0 &&
      shb_type("/proc/meminfo") == 0) cnt++;
  SLEEP(100);
  char path[256];
  sprintf(path, "/proc/%d", this_thread->tid);
  if (sh_access(path, R_OK) == 0 && shb_type(path) == 0) cnt++; 

  VERDICT(!(chk_cnt == 4 && cnt == 3), 
      "%d of 4 checkpoints passed, %d of 3 files succeeded", 
      chk_cnt, cnt);
}

