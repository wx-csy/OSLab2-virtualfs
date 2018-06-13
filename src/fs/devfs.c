#include <fs/devfs.h>

// #define DEBUG_ME
#include <debug.h>

static int _ctor(struct filesystem *fs, const char *name);
static inode_t lookup(struct filesystem *fs, const char *path);
static inode_t create(struct filesystem *fs, const char *path);
static int access(struct filesystem *fs, inode_t inode, int mode);
static file_t *open(struct filesystem *fs, inode_t inode, int flags);
static void _dtor(struct filesystem *fs);

Implementation(filesystem, devfs) = {
  ._ctor = _ctor, 
  .lookup = lookup,
  .create = create,
  .access = access,
  .open = open,
  ._dtor = _dtor
};

static int _ctor(struct filesystem *_fs, const char *name) {
  devfs_t *fs = (devfs_t *)_fs;
  
}

static inode_t lookup(struct filesystem *_fs, const char *path) {
  devfs_t *fs = (devfs_t *)_fs;

}

static inode_t create(struct filesystem *_fs, const char *path) {
  // this file system does not support creating new files
  return -1;
}

static int access(struct filesystem *_fs, inode_t inode, int mode) {
  devfs_t *fs = (devfs_t *)_fs;
  return mode == (fs.devices[inode].mode & mode) ? 0 : -1;
}

static file_t *open(struct filesystem *fs, inode_t inode, int flags) {
  devfs_t *fs = (devfs_t *)_fs;
   
}

static void _dtor(struct filesystem *fs) {
  devfs_t *fs = (devfs_t *)_fs;

}

