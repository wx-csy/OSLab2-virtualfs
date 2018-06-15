#include <kernel.h>
#include <os.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

static spinlock_t test_lock;
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
      printf(" : fs = \"%s\", inode = %d, refcnt = %d, offset = %d, "
          "flags = %d\n",
          fp->fs->name, fp->inode, fp->refcnt, fp->offset, fp->flags);
    } else {
      printf("\n");
    }
  }
}

static int lsfile(const char *path, inode_t inode, 
    int mode, int length) {
  char smode[4] = {'-', '-', '\0'};
  if (mode & R_OK) smode[0] = 'r';
  if (mode & W_OK) smode[1] = 'w';
  printf("%5d %s %5d %s\n", inode, smode, length, path);
  return 0;
}

static int lsfs(const char *path, filesystem_t *fs) {
  printf("[%s] %s\n", fs->name, path);
  Invoke(fs, walk, lsfile);
  printf("\n");
  return 0;
}

static void cmd_ls(char *args) {
  vfs->fsls(lsfs);  
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
       *s_flags = strtok(NULL, " ");
  int flags;
  if (s_flags == NULL) flags = O_RDWR; else flags = atoi(s_flags);
  printf("open(\"%s\", %d) = %d\n", path, flags, 
      vfs->open(path, flags));
}

static void cmd_close(char *args) {
  int fd = atoi(args);

  printf("close(fd = %d) = %d\n", fd, vfs->close(fd));
}

static void cmd_write(char *args) {
  char *s_fd = strtok(args, " "),
       *content = strtok(NULL, " ");
  int fd = atoi(s_fd);
  printf("write(fd = %d, \"%s\") = %d\n", fd, content,
      vfs->write(fd, content, strlen(content)));
}

static void cmd_type(char *args) {
  int fd = kmt->open(args, O_RDONLY);
  char buf[4096];
  int sz = kmt->read(fd, buf, sizeof buf);
  buf[sz] = 0;
  printf("%s", buf);
}

struct cmd {
  const char *cmd;
  void (*func)(char *args);
} cmds[] = {
  {"exit", cmd_exit},
  {"fd", cmd_fd},
  {"token", cmd_token},
  {"open", cmd_open},
  {"close", cmd_close},
  {"ls", cmd_ls},
  {"write", cmd_write},
};

#define NR_CMD  (sizeof(cmds) / sizeof(struct cmd))

void devfs_test(void *igonre) {
  kmt->spin_init(&test_lock, "test_lock");
  fd = vfs->open("/dev/stdin", O_RDONLY);
  int data = 0;
  printf("Welcome to OSLab2!\n");
  while (1) {
    printf("$ ");
    char buf[256];
    term_getline(buf);
    kmt->spin_lock(&test_lock);
    char *cmd = strtok(buf, " ");
    for (int i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmds[i].cmd) == 0) {
        cmds[i].func(cmd + strlen(cmd) + 1);
        goto next;
      }
    } 
    printf("Command `%s' not found!\n", cmd);
next:;
    kmt->spin_unlock(&test_lock);
  }
  printf("random: %d\n", data);
}

