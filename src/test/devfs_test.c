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
  } while (buf[sz - 1] != '\n');
  buf[sz - 1] = 0;
}

static void cmd_exit() {
  _Exit(0);
}

static void cmd_fd() {
  for (int i = 0; i < 16; i++) {
    printf("fd[%d] = %p\n", i, this_thread->fd[i]);
  }
}

struct cmd {
  const char *cmd;
  void (*func)();
} cmds[] = {
  {"exit", cmd_exit},
  {"fd", cmd_fd}
};

#define NR_CMD  (sizeof(cmds) / sizeof(struct cmd))

void devfs_test(void *igonre) {
  fd = vfs->open("/dev/stdin", O_RDONLY);
  int data = 0;
  printf("Welcome to OSLab2!\n");
  while (1) {
    printf("$ ");
    char buf[256];
    term_getline(buf);
    printf("%s\n", buf);
    for (int i = 0; i < NR_CMD; i++) {
      if (strcmp(buf, cmds[i].cmd) == 0) {
        cmds[i].func();
        goto next;
      }
    } 
    printf("Command `%s' not found!", buf);
next:;
  }
  printf("random: %d\n", data);
}

