#include <kernel.h>
#include <fs/kvfs.h>
#include <string.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

static int _ctor(file_t *file, filesystem_t *fs, inode_t inode, int flags);
static ssize_t read(file_t *file, char *buf, size_t size);
static ssize_t write(file_t *file, const char *buf, size_t size);
static off_t lseek(file_t *file, off_t offset, int whence);
static void _dtor(file_t *file);

Implementation(file, devfile) = {
  ._ctor = _ctor, 
  .read = read,
  .write = write,
  .lseek = lseek,
  ._dtor = _dtor
};

static int _ctor(file_t *file, filesystem_t *fs, inode_t inode, int flags) {
  file->fs = fs;
  file->inode = inode;
  file->refcnt = 0;
  file->offset = 0;
  file->flags = flags;
  return 0;
}

static ssize_t read(file_t *file, char *buf, size_t size) {
  return size;
}

static ssize_t write(file_t *file, const char *buf, size_t size) {
  return size;  
}

static int lseek(file_t *file, off_t offset, int whence) {
  return -1;
}

static void _dtor(file_t *file) {
  assert(file->refcnt == 0);
  return;
} 

