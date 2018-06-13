#include <kernel.h>
#include <fs/kvfs.h>
#include <string.h>

// #define DEBUG_ME
#include <debug.h>

static int _ctor(struct filesystem *fs, const char *name);
static inode_t lookup(struct filesystem *fs, const char *path);
static inode_t create(struct filesystem *fs, const char *path);
static int access(struct filesystem *fs, inode_t inode, int mode);
static file_t *open(struct filesystem *fs, inode_t inode, int flags);
static void _dtor(struct filesystem *fs);

Implementation(filesystem, kvfs) = {
  ._ctor = _ctor, 
  .lookup = lookup,
  .create = create,
  .access = access,
  .open = open,
  ._dtor = _dtor
};

static int _ctor(struct filesystem *_fs, const char *name) {
  strncpy(_fs->name, name, sizeof _fs->name);
  _fs->name[31] = 0; 
  _fs->refcnt = 0;
  kvfs_t *fs = (kvfs_t *)_fs;
  for (int i = 0; i < MAX_KVP; i++) {
    fs->kvp[i].valid = 0;
  }
  return 0;
}

static inode_t lookup(struct filesystem *_fs, const char *path) {
  kvfs_t *fs = (kvfs_t *)_fs;
  for (int i = 0; i < MAX_KVP; i++) {
    if (!fs->kvp[i].valid) continue;
    if (strcmp(path, fs->kvp[i].key) == 0) return i;
  } 
  return -1;
}

static inode_t create(struct filesystem *_fs, const char *path) {
  if (strlen(path) > 31) {
_debug("The key is too long!");
    return -1;
  }
  kvfs_t *fs = (kvfs_t *)_fs;
  for (int i = 0; i < MAX_KVP; i++) {
    if (fs->kvp[i].valid) continue;
    strcpy(fs->kvp[i].key, path);
    fs->kvp[i].data = pmm->alloc(512);
    if (fs->kvp[i].data == NULL) {
_debug("Fail to allocate memory!");
      return -1;
    }
    fs->kvp[i].valid = 1;
    fs->kvp[i].capacity = 512;
    fs->kvp[i].length = 0;
    return i;
  }
_debug("No free space available!");
  return -1;
}

static int access(struct filesystem *_fs, inode_t inode, int mode) {
  kvfs_t *fs = (kvfs_t *)_fs;
  return fs->kvp[inode].valid ? 0 : -1;
}

static file_t *open(struct filesystem *_fs, inode_t inode, int flags) {
  kvfs_t *fs = (kvfs_t *)_fs;
  
  return NULL; 
}

static int _dtor(struct filesystem *_fs) {
  if (_fs.refcnt) return -1;
  devfs_t *fs = (devfs_t *)_fs;
  for (int i = 0; i < MAX_KVP; i++) {
    if (!fs->kvp[i].valid) continue;
    pmm->free(fs->kvp[i].data);
    fs->kvp[i].valid = 0;
  }
  return 0;
}

