#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void devfs_test(void *igonre) {
  int fd = vfs->open("/dev/stdin", O_RDONLY);
  int data = 0;
  printf("Welcome to OSLab2!\n");
  while (1) {
    printf("> ");
    do {
      while (vfs->read(fd, &data, 1) == 0);
    } while (data != '\n');
    _putc('\n');
//    _putc(data);
  }
  printf("random: %d\n", data);
}

