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
  if ((fd = sh_open("/dev/null", O_RDWR)) >= 0) chk_cnt++;
  if (sh_write(fd, buf, sizeof buf) == sizeof buf) chk_cnt++;
  if (sh_read(fd, buf, sizeof buf) == 0) chk_cnt++;

  VERDICT(!(chk_cnt == 3), "%d of 3 checkpoints passed", chk_cnt);
}

