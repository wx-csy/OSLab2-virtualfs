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
  
  // 3 test points 
  if (sh_mount("/", New(kvfs, "kvfs_fail")) < 0) chk_cnt++;
  if (sh_mount("/test/", New(kvfs, "kvfs_test")) >= 0) chk_cnt++;
  if (sh_access("/test/inexist", R_OK | W_OK) < 0) chk_cnt++;
  
  // 8 test points
  int fd = sh_open("/test/file0", O_RDWR);
  if (fd >= 0) chk_cnt++;
  char buf[512];
  if (sh_read(fd, buf, 10) == 0) chk_cnt++;
  const char test_text[] = "the quick brown fox jumps over a lazy dog\n";
  if (sh_write(fd, test_text, sizeof test_text) == sizeof test_text) chk_cnt++;
  if (sh_close(fd) == 0) chk_cnt++;
  if ((fd = sh_open("/test/file0", O_RDONLY)) >= 0) chk_cnt++;
  memset(buf, 0, sizeof buf);
  sh_read(fd, buf, sizeof buf);
  if (strcmp(buf, test_text) == 0) chk_cnt++;
  if (sh_close(fd) == 0) chk_cnt++;

  // 8 test points
  fd = sh_open("/test/file1", O_WR);
  if (fd >= 0) chk_cnt++;
  if (sh_lseek(fd, 4, SEEK_SET) == 4) chk_cnt++;
  if (sh_write(fd, "456789", 6) == 6) chk_cnt++;
  if (sh_lseek(fd, -4, SEEK_CUR) == 0) chk_cnt++;
  if (sh_write(fd, "abcde", 5) == 5) chk_cht++;
  if (sh_lseek(fd, -1, SEEK_END) == 9) chk_cnt++;
  if (sh_write(fd, "@!#$%^&", 7) == 7) chk_cnt++;
  if (sh_close(fd) == 0) chk_cnt++;

  // 3 test points
  shb_ls();
  if (shb_type("/test/file0") == 0) chk_cnt++;

  
  if (sh_unmount("/test/") == 0) chk_cnt++;
  
  VERDICT(chk_cnt == 22 ? 0 : 1,
      "%d of 22 checkpoints passed", chk_cnt);
}

