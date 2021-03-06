#include <fs/devfs.h>
#include <fs/devices.h>
#include <pmr.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <os.h>
#include <kernel.h>

// #define DEBUG_ME
#include <debug.h>

static void init();
static int access(const char *path, int mode);
static int mount(const char *path, filesystem_t *fs);
static int unmount(const char *path);
static int fsls(int (*fn)(const char *path, filesystem_t *fs));
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
  .fsls = fsls,
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
  int ret = -1, maxlen = 0;
  for (int i = 0; i < NR_MOUNTPOINTS; i++) {
    if (!mounts[i].valid) continue;
    int pos = 0;
    for (pos = 0; mounts[i].path[pos]; pos++) {
      if (path[pos] != mounts[i].path[pos]) goto next;
    }
    if (pos > maxlen) {
      maxlen = pos;
      ret = i;
    }
next:;
  }
  return ret;
}

static void init() {
  kmt->spin_init(&vfs_lock, "vfs_lock");
  dev_init();
}

static int mount(const char *path, filesystem_t *fs) {
_debug("mounting `%s' to %s", fs->name, path);
  if (strlen(path) >= MAX_PATH) {
_debug("The length of path exceeds the limit (max %d).", MAX_PATH);
    Delete(fs);
    return -1;
  }
LOCK
  int fsid = find_fs(path);
  if (fsid >= 0 && strcmp(mounts[fsid].path, path) == 0) {
_debug("A filesystem has already been mounted to the path");
UNLOCK
    Delete(fs);
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
  Delete(fs);
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
  if (mounts[id].fs->refcnt) {
_debug("The filesystem is still buzy! (refcnt = %d)", mounts[id].fs->refcnt);
UNLOCK
    return -1;
  }
  Delete(mounts[id].fs);
  mounts[id].valid = 0;
UNLOCK
  return 0;
}

static int fsls(int (*fn)(const char *path, filesystem_t *fs)) {
LOCK
  for (int i = 0; i < NR_MOUNTPOINTS; i++) {
    if (mounts[i].valid) {
      int val = fn(mounts[i].path, mounts[i].fs);
      if (val != 0) {
UNLOCK
        return val;
      }
    }
  }
UNLOCK
  return 0;
}

static int access(const char *path, int mode) {
  if (strlen(path) >= MAX_PATH) {
_debug("The length of path exceeds the limit (max %d).", MAX_PATH);
    return -1;
  }
LOCK
  int fsid = find_fs(path);
  if (fsid < 0) {
_debug("Filesystem not found!");
UNLOCK
    return -1;
  }
  const char *relpath = path + strlen(mounts[fsid].path);
  filesystem_t *fs = mounts[fsid].fs;
  inode_t inode = Invoke(fs, lookup, relpath);
  if (inode < 0) {
_debug("File not found!");
UNLOCK
    return -1;
  }
  int ret = Invoke(fs, access, inode, mode);
UNLOCK
  return ret;
}

static int open(const char *path, int flags) {
_debug("Opening `%s' with flags=%d", path, flags);
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
    inode_t inode = Invoke(fs, lookup, relpath);
    if (inode < 0) { 
_debug("Attempting to create file!");
      inode = Invoke(fs, create, relpath);
    }
    if (inode < 0) {
_debug("File not found!");
UNLOCK
      return -1;
    }
    file_t *file = Invoke(fs, open, inode, flags);
    if (file == NULL) {
UNLOCK
      return -1;
    }
    this_thread->fd[i] = file;
    fs->refcnt++;
UNLOCK
    return i;
  }
_debug("No free file descriptor available!");
UNLOCK
  return -1;
}

static ssize_t read(int fd, void *buf, size_t nbyte) {
LOCK
  int ret = Invoke(this_thread->fd[fd], read, buf, nbyte);
UNLOCK
  return ret;
}

static ssize_t write(int fd, const void *buf, size_t nbyte) {
LOCK
  int ret = Invoke(this_thread->fd[fd], write, buf, nbyte);
UNLOCK
  return ret;
}

static off_t lseek(int fd, off_t offset, int whence) {
LOCK
  off_t ret = Invoke(this_thread->fd[fd], lseek, offset, whence);
UNLOCK
  return ret;
}

static int close(int fd) {
LOCK
  this_thread->fd[fd]->refcnt--;
  if (this_thread->fd[fd]->refcnt == 0) {
    this_thread->fd[fd]->fs->refcnt--;
    Delete(this_thread->fd[fd]);
  }
  this_thread->fd[fd] = NULL;
UNLOCK
  return 0;
}

