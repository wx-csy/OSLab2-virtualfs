#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void devfs_test(void *igonre) {
  int fd = vfs->open("/dev/stdin", O_RDONLY);
  int data = 0;
  printf("Welcome to OSLab2!\n")
  while (1) {
    printf("> ");
    vfs->read(fd, &data, 1);
//    _putc(data);
  }
  printf("random: %d\n", data);
}

