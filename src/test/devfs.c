#include <kernel.h>
#include <os.h>
#include <test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

int test_devfs() {
  puts("====== devfs Functional Test ======");
  puts("# test functions of devfs");
  int chk_cnt = 0;
  
  int buf[128];
  srand(time(NULL));
  for (int i = 0; i < 128; i++) buf[i] = rand();
  
  int fd;

  SLEEP(1000);

  // 5 checkpoints for /dev/null
  if (sh_access("/dev/null", R_OK | W_OK) == 0) chk_cnt++;
  if ((fd = sh_open("/dev/null", O_RDWR)) >= 0) chk_cnt++;
  if (sh_write(fd, buf, sizeof buf) == sizeof buf) chk_cnt++;
  if (sh_read(fd, buf, sizeof buf) == 0) chk_cnt++;
  if (sh_close(fd) == 0) chk_cnt++;

  printf("chk_cnt=%d\n", chk_cnt);
  SLEEP(1000);

  // 6 checkpoints for /dev/zero
  if (sh_access("/dev/zero", R_OK | W_OK) == 0) chk_cnt++;
  if ((fd = sh_open("/dev/zero", O_RDWR)) >= 0) chk_cnt++;
  if (sh_write(fd, buf, sizeof buf) == sizeof buf) chk_cnt++;
  if (sh_read(fd, buf, sizeof(int)) == 0) chk_cnt++;
  if (buf[0] == 0) chk_cnt++;
  if (sh_close(fd) == 0) chk_cnt++;
  
  printf("chk_cnt=%d\n", chk_cnt);
  SLEEP(1000);

  // 5 checkpoints for /dev/random
  if (sh_access("/dev/random", R_OK | W_OK) < 0) chk_cnt++;
  if (sh_access("/dev/random", R_OK) == 0) chk_cnt++;
  if ((fd = sh_open("/dev/random", O_RDONLY)) >= 0) chk_cnt++;
  if (sh_read(fd, buf, sizeof(int)) == sizeof(int)) chk_cnt++;
  printf("random number read: %d\n", buf[0]);
  if (sh_close(fd) == 0) chk_cnt++;

  VERDICT(!(chk_cnt == 16), "%d of 16 checkpoints passed", chk_cnt);
}

