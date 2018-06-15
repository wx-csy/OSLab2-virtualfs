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

static void cmd_exit(char *args) {
  _Exit(0);
}

static void cmd_fd(char *args) {
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

static void cmd_token(char *args) {
  char buf[256];
  term_getline(buf);
  char *token = strtok(buf, " :|");
  while (token) {
    puts(token);
    token = strtok(NULL, " :|");
  }
}

static void cmd_open(char *args) {
  char *path = strtok(args, " "), 
       *s_flags = strtok(path + strlen(path) + 1, " ");
  int flags;
  if (s_flags == NULL) flags = O_RDWR; else flags = atoi(s_flags);
  printf("open(\"%s\", %d) = %d", path, s_flags, 
      vfs->open(path, flags));
}

struct cmd {
  const char *cmd;
  void (*func)(char *args);
} cmds[] = {
  {"exit", cmd_exit},
  {"fd", cmd_fd},
  {"token", cmd_token},
  {"open", cmd_open},
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
    char *cmd = strtok(buf, " ");
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

