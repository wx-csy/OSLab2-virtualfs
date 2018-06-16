#include <kernel.h>
#include <string.h>
#include <assert.h>
#include <fs/procfs.h>

// #define DEBUG_ME
#include <debug.h>

static int      _ctor Member (filesystem_t *fs, inode_t inode, int flags);
static ssize_t  read  Member (char *buf, size_t size);
static ssize_t  write Member (const char *buf, size_t size);
static off_t    lseek Member (off_t offset, int whence);
static void     _dtor Member ();

Implementation(file, otrofile) = {
  ._ctor  = _ctor, 
  .read   = read,
  .write  = write,
  .lseek  = lseek,
  ._dtor  = _dtor
};  

static int _ctor Member (filesystem_t *fs, inode_t inode, int flags) {
  MemberOf(otrofile);
  
  base.fs = fs;
  base.inode = inode;
  base.refcnt = 0;
  base.offset = 0;
  base.flags = flags;

  this.length = 0;
  this.data = NULL;
  return 0;
}

static ssize_t read Member (char *buf, size_t size) {
  MemberOf(otrofile);
  
  if ((this.flags & O_RD) == 0) return -1;
  if (base.offset + size > this.length) {
    size = this.length - base.offset;
    memcpy(buf, this.data + base.offset, size);
    base.offset = this.length;
    return 0;
  } else {
    memcpy(buf, this.data + base.offset, size);
    base.offset += size;
    return size; 
  }
}

static ssize_t write Member (const char *buf, size_t size) {
  MemberOf(otrofile);
  
  // this type of file does not support write 
  return -1;
}

static int lseek Member (off_t offset, int whence) {
  MemberOf(otrofile);
  
  switch (whence) {
    case SEEK_SET:
      break;
    case SEEK_CUR:
      offset += base.offset;
      break;
    case SEEK_END:
      offset += this.length;
      break;
    default:
_debug("Invalid whence parameter (%d)", whence);
      return -1;
  }
  if (offset < 0 || offset > this.length) {
_debug("Offset out of range!");
    return -1;
  }
  base.offset = offset;
  return offset;
}

static void _dtor Member () {
  MemberOf(otrofile);
    
  assert(base.refcnt == 0);
  pmm->free(this.data);
  return;
} 

