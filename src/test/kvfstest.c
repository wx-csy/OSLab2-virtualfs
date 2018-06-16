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
  
  if (sh_mount("/", New(kvfs, "kvfs_fail")) < 0) chk_cnt++;
  if (sh_mount("/test/", New(kvfs, "kvfs_test")) >= 0) chk_cnt++;
  if (sh_access("/test/inexist", R_OK | W_OK) < 0) chk_cnt++;
  
  int fd = sh_open("/test/file0", O_RDWR);
  char buf[512];
  if (sh_read(fd, buf, 10) == 0) chk_cnt++;

  if (sh_unmount("/test/") == 0) chk_cnt++;

  VERDICT(chk_cnt == 2 ? 0 : 1,
      "%d of 2 checkpoints passed", chk_cnt);
}

