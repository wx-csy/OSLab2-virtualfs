#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <os.h>

// #define DEBUG_ME
#include <debug.h>

static void init();
static int access(const char *path, int mode);
static int mount(const char *path, filesystem_t *fs);
static int unmount(const char *path);
static int open(const char *path, int flags);
static ssize_t read(int fd, void *buf, size_t nbyte);
static ssize_t write(int fd, const void *buf, size_t nbyte);
static off_t lseek(int fd, off_t offset, int whence);
static int close(int fd);

MOD_DEF(vfs) {
  .init = init,
  .access = access,
  .mount = mount,
  .unmount = unmount,
  .open = open,
  .read = read,
  .write = write,
  .lseek = lseek,
  .close = close,
};

#define NR_MOUNTPOINTS 16
#define MAX_PATH  128

static struct {
  int valid;
  char path[MAX_PATH];
  filesystem_t *fs;
} mounts[NR_MOUNTPOINTS];

static spinlock_t vfs_lock;
#define LOCK    kmt->spin_lock(&vfs_lock);
#define UNLOCK  kmt->spin_unlock(&vfs_lock);

static int find_fs(const char *path) {
  for (int i = 0; i < NR_MOUNTPOINTS; i++) {
    if (!mounts[i].valid) continue;
    for (int pos = 0; mounts[i].path[pos]; pos++) {
      if (path[pos] != mounts[i].path[pos]) goto next;
    }
    return i;
next:;
  }
  return -1;
}

static void init() {
  kmt->spin_init(&vfs_lock, "vfs_lock");

}

static int access(const char *path, int mode) {
LOCK
  
UNLOCK
  return 0;
}

static int mount(const char *path, filesystem_t *fs) {
  if (strlen(path) >= MAX_PATH) {
_debug("The length of path exceeds the limit (max %d).", MAX_PATH);
    return -1;
  }
LOCK
  if (find_fs(path) >= 0) {
_debug("A filesystem has already been mounted to the path");
UNLOCK
    return -1;
  }
  for (int i = 0; i < NR_MOUNTPOINTS; i++) {
    if (mounts[i].valid) continue;
    mounts[i].valid = 1;
    strcpy(mounts[i].path, path);
    mounts[i].fs = fs;
UNLOCK
    return i;
  }
_debug("The mount list is full.");
UNLOCK
  return -1;
}

static int unmount(const char *path) {
  if (strlen(path) >= MAX_PATH) {
_debug("The length of path exceeds the limit (max %d).", MAX_PATH);
    return -1;
  }
LOCK
  int id = find_fs(path);
  if (id < 0 || strcmp(path, mounts[id].path)) {
_debug("There is no filesystem mounted to the path.");
UNLOCK
    return -1;
  }
  mounts[id].valid = 0;
UNLOCK
  return 0;
}

static int open(const char *path, int flags) {
LOCK
  for (int i = 0; i < MAX_FD; i++) {
    if (this_thread->fd[i] != NULL) continue;
    int fsid = find_fs(path);
    if (fsid < 0) {
_debug("Filesystem not found!");
UNLOCK
      return -1;
    }
    const char *relpath = path + strlen(mounts[fsid].path);
    filesystem_t *fs = mounts[fsid].fs;
    inode_t inode = PInvoke(fs, lookup, relpath);
    if (inode < 0) 
      inode = PInvoke(fs, create, relpath);
    if (inode < 0) {
_debug("File not found!");
UNLOCK
      return -1;
    }
    file_t *file = PInvoke(fs, open, inode);
    if (file == NULL) {
UNLOCK
      return -1;
    }
    this_thread->fd[i] = file;
UNLOCK
    return i;
  }
_debug("No free file descriptor available!");
UNLOCK
  return -1;
}

static ssize_t read(int fd, void *buf, size_t nbyte) {
LOCK
  int ret = PInvoke(this_thread->fd[fd], read, buf, nbyte);
UNLOCK
  return ret;
}

static ssize_t write(int fd, const void *buf, size_t nbyte) {
LOCK
  int ret = PInvoke(this_thread->fd[fd], write, buf, nbyte);
UNLOCK

}

static off_t lseek(int fd, off_t offset, int whence) {
LOCK
  off_t ret = PInvoke(this_thread->fd[fd], lseek, offset, whence);
UNLOCK
  return ret;
}

static int close(int fd) {
LOCK
  this_thread->fd[fd]->refcnt--;
  if (this_thread->fd[fd]->refcnt == 0) {
    pmm->free(this_thread->fd[fd]);
    this_thread->fd[fd] = NULL;
  }
UNLOCK
  return 0;
}

