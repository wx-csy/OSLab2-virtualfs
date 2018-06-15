#include <kernel.h>
#include <os.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static void cmd_exit(const char *args) {
  _Exit(0);
}

static void cmd_fd(const char *args) {
  for (int i = 0; i < 16; i++) {
    file_t *fp = this_thread->fd[i];
    printf("fd[%d] -> %p", i, fp);
    if (fp) {
      printf(" : inode = %d, refcnt = %d, offset = %d, flags = %d\n",
          fp->inode, fp->refcnt, fp->offset, fp->flags);
    } else {
      printf("\n");
    }
  }
}

static void cmd_token(const char *args) {
  char buf[256];
  term_getline(buf);
  char *token = strtok(buf, " :|");
  while (token) {
    puts(token);
    token = strtok(NULL, " :|");
  }
}

struct cmd {
  const char *cmd;
  void (*func)(const char *args);
} cmds[] = {
  {"exit", cmd_exit},
  {"fd", cmd_fd},
  {"token", cmd_token}
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
    const char *cmd = strtok(buf, " ");
    for (int i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmds[i].cmd) == 0) {
        cmds[i].func(cmd + strlen(cmd) + 1);
        goto next;
      }
    } 
    printf("Command `%s' not found!\n", cmd);
next:;
  }
  printf("random: %d\n", data);
}

