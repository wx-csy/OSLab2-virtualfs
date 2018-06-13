#include <fs/devfs.h>
#include <string.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

static int _ctor(filesystem_t *fs, const char *name);
static inode_t lookup(filesystem_t *fs, const char *path);
static inode_t create(filesystem_t *fs, const char *path);
static int access(filesystem_t *fs, inode_t inode, int mode);
static file_t *open(filesystem_t *fs, inode_t inode, int flags);
static void _dtor(filesystem_t *fs);

Implementation(filesystem, devfs) = {
  ._ctor = _ctor, 
  .lookup = lookup,
  .create = create,
  .access = access,
  .open = open,
  ._dtor = _dtor
};

static void load_device(struct device *dev, const char *name, int (*getch)(), 
    int (*putch)(int ch)) {
  dev->valid = 1;
  strcpy(dev->name, name);
  dev->mode = 0;
  if (getch != NULL) dev->mode |= R_OK;
  if (putch != NULL) dev->mode |= W_OK;
  dev->getch = getch;
  dev->putch = putch;
}

static int _ctor(filesystem_t *_fs, const char *name) {
  strncpy(_fs->name, name, sizeof _fs->name);
  _fs->name[(sizeof _fs->name) - 1] = 0;
  devfs_t *fs = (devfs_t *)_fs;
  for (int i = 0; i < MAX_DEV; i++) {
    fs->devices[i].valid = 0;
  }
  load_device(&(fs->devices[0]), "null", dev_null_getch, dev_null_putch);
  load_device(&(fs->devices[1]), "zero", dev_zero_getch, dev_null_putch);
  load_device(&(fs->devices[2]), "random", dev_random_getch, NULL);
  return 0;  
}

static inode_t lookup(filesystem_t *_fs, const char *path) {
  devfs_t *fs = (devfs_t *)_fs;
  for (int i = 0; i < MAX_DEV; i++) {
    if (!fs->devices[i].valid) continue;
    if (strcmp(path, fs->devices[i].name)) continue;
    return i;
  }
_debug("Device not found!");
  return -1;
}

static inode_t create(filesystem_t *_fs, const char *path) {
  // this file system does not support creating new files
  return -1;
}

static int access(filesystem_t *_fs, inode_t inode, int mode) {
  devfs_t *fs = (devfs_t *)_fs;
  return mode == (fs->devices[inode].mode & mode) ? 0 : -1;
}

static file_t *open(filesystem_t *_fs, inode_t inode, int flags) {
//  devfs_t *fs = (devfs_t *)_fs;
  file_t *file = pmm->alloc(sizeof(file_t));
  if (file == NULL) {
_debug("Memory allocation failed!");
    return NULL;
  }
  PMR_Init(file, devfile);
  if (Invoke(file, _ctor, _fs, inode, flags) != 0) {
    pmm->free(file);
    return NULL; 
  }
  file->refcnt++;
  return file;
}

static void _dtor(filesystem_t *_fs) {
  assert(_fs->refcnt == 0);
//  devfs_t *fs = (devfs_t *)_fs;
  return ;
}

