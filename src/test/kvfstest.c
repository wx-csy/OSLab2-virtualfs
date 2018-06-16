#include <kernel.h>
#include <os.h>
#include <test.h>
#include <pmr.h>
#include <fs/kvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

int test_kvfstest() {
  puts("====== kvfs Functional Test ======");
  puts("# test functions of kvfs");
  int chk_cnt = 0;
  
  if (sh_mount("/test/", NEW(kvfs, "kvfs_test")) == 0) chk_cnt++;
  if (sh_unmount("/test/") == 0) chk_cnt++;
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

