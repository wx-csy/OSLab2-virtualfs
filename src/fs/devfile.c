#include <kernel.h>
#include <fs/devfs.h>
#include <string.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

static int      _ctor Member (filesystem_t *fs, inode_t inode, int flags);
static ssize_t  read  Member (char *buf, size_t size);
static ssize_t  write Member (const char *buf, size_t size);
static off_t    lseek Member (off_t offset, int whence);
static void     _dtor Member ();

Implementation(file, devfile) = {
  ._ctor = _ctor, 
  .read = read,
  .write = write,
  .lseek = lseek,
  ._dtor = _dtor
};

static int _ctor Member (filesystem_t *fs, inode_t inode, int flags) {
  MemberOf(devfile);
  
  base.fs = fs;
  base.inode = inode;
  base.refcnt = 0;
  base.offset = 0;
  base.flags = flags;
  return 0;
}

static ssize_t read Member (char *buf, size_t size) {
  MemberOf(devfile);

  return size;
}

static ssize_t write Member (const char *buf, size_t size) {
  MemberOf(devfile);

  return size;  
}

static int lseek Member (off_t offset, int whence) {
  MemberOf(devfile);

  // devices do not support lseek
  return -1;
}

static void _dtor Member () {
  MemberOf(devfile);
    
  assert(file->refcnt == 0);
  return;
} 

