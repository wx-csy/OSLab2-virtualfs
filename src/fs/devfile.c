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
  ._ctor  = _ctor, 
  .read   = read,
  .write  = write,
  .lseek  = lseek,
  ._dtor  = _dtor
};  

static int _ctor Member (filesystem_t *fs, inode_t inode, int flags) {
  MemberOf(devfile);
  
  this.devfs = (devfs_t *)fs;
  base.fs = fs;
  base.inode = inode;
  base.refcnt = 0;
  base.offset = 0;
  base.flags = flags;
  return 0;
}

static ssize_t read Member (char *buf, size_t size) {
  MemberOf(devfile);
  
  if ((base.flags & O_RDONLY) == 0) return -1; 
  struct device *dev = &(this.devfs->devices[base.inode]);
  if (dev->getch == NULL) return -1;
  size_t cnt = 0;
  while (size) {
    int tmp = dev->getch();
    if (tmp == -1) return 0;
    else if (tmp == -2) return cnt;
    else if (tmp < 0 || tmp > 0xff) return -1;
    else {
      *buf = tmp;
      buf++;
      cnt++;
      size--;
    }
  }
  return cnt;
}

static ssize_t write Member (const char *buf, size_t size) {
  MemberOf(devfile);
  
  if ((base.flags & O_WRONLY) == 0) return -1;
  struct device *dev = &(this.devfs->devices[base.inode]);
  if (dev->putch == NULL) return -1;
  size_t cnt = 0;
  while (size) {
    if (dev->putch(*buf) != 0) return cnt;
    buf++;
    cnt++;
    size--;
  }
  return cnt;
}

static int lseek Member (off_t offset, int whence) {
  MemberOf(devfile);

  // devices do not support lseek
  return -1;
}

static void _dtor Member () {
  MemberOf(devfile);
    
  assert(base.refcnt == 0);
  return;
} 

