#include <pmr.h>
#include <kernel.h>
#include <fs/kvfs.h>
#include <string.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

static int      _ctor   Member (const char *name);
static int      walk    Member 
    (int (*fn)(const char *path, inode_t inode, int mode, int length));
static inode_t  lookup  Member (const char *path);
static inode_t  create  Member (const char *path);
static int      access  Member (inode_t inode, int mode);
static file_t * open    Member (inode_t inode, int flags);
static void     _dtor   Member ();

Implementation(filesystem, kvfs) = {
  ._ctor = _ctor, 
  .walk = walk,
  .lookup = lookup,
  .create = create,
  .access = access,
  .open = open,
  ._dtor = _dtor
};

static int _ctor Member (const char *name) {
  MemberOf(kvfs);

  strncpy(base.name, name, sizeof base.name);
  base.name[(sizeof base.name) - 1] = 0; 
  base.refcnt = 0;

  for (int i = 0; i < MAX_KVP; i++) {
    this.kvp[i].valid = 0;
  }

  return 0;
}

static int walk Member 
    (int (*fn)(const char *path, inode_t inode, int mode, int length)) {
  MemberOf(kvfs);

  for (int i = 0; i < MAX_KVP; i++) {
    if (this.kvp[i].valid) {
      int val = fn(this.kvp[i].key, i, R_OK | W_OK, this.kvp[i].length);
      if (val != 0) return val;
    }
  }  
  return 0;
}

static inode_t lookup Member (const char *path) {
  MemberOf(kvfs);

  for (int i = 0; i < MAX_KVP; i++) {
    if (!this.kvp[i].valid) continue;
    if (strcmp(path, this.kvp[i].key) == 0) return i;
  } 
  return -1;
}

static inode_t create Member (const char *path) {
  MemberOf(kvfs);

  if (strlen(path) > 31) {
_debug("The key is too long!");
    return -1;
  }
  
  for (int i = 0; i < MAX_KVP; i++) {
    if (this.kvp[i].valid) continue;
    strcpy(this.kvp[i].key, path);
    this.kvp[i].data = pmm->alloc(512);
    if (this.kvp[i].data == NULL) {
_debug("Fail to allocate memory!");
      return -1;
    }
    this.kvp[i].valid = 1;
    this.kvp[i].capacity = 512;
    this.kvp[i].length = 0;
    return i;
  }
_debug("No free space available!");
  return -1;
}

static int access Member (inode_t inode, int mode) {
  MemberOf(kvfs);

  return this.kvp[inode].valid ? 0 : -1;
}

static file_t *open Member (inode_t inode, int flags) {
  MemberOf(kvfs);
//  kvfs_t *fs = (kvfs_t *)_fs;
  
  return NULL; 
}

static void _dtor Member () {
  MemberOf(kvfs);

  assert(base.refcnt == 0);
  for (int i = 0; i < MAX_KVP; i++) {
    if (this.kvp[i].valid) continue;
    pmm->free(this.kvp[i].data);
    this.kvp[i].valid = 0;
  }
}

