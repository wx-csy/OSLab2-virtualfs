#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

int fd;

static void term_getline(char *buf) {
  int sz = 0;
  do {
    sz += vfs->read(fd, buf, 100);
    printf("sz=%d, buf[sz-1]=%c\n", sz, buf[sz-1]);
  } while (buf[sz - 1] != '\n');
  buf[sz - 1] = 0;
}

void devfs_test(void *igonre) {
  fd = vfs->open("/dev/stdin", O_RDONLY);
  int data = 0;
  printf("Welcome to OSLab2!\n");
  while (1) {
    printf("> ");
    char buf[256];
    term_getline(buf);
    printf("%s\n", buf);
  }
  printf("random: %d\n", data);
}

