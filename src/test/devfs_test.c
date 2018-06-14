#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void devfs_test(void *igonre) {
  int fd = vfs->open("/dev/random", O_RDONLY);
  int data;
  vfs->read(fd, &data, sizeof data);
  printf("random: %d\n", data);
}

