#include <kernel.h>
#include <fs/kvfs.h>
#include <string.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

static int      _ctor Member (filesystem_t *fs, inode_t inode, int flags);
static ssize_t  read  Member (char *buf, size_t size);
static ssize_t  write Member (const char *buf, size_t size);
static off_t    lseek Member (off_t offset, int whence);
static void     _dtor Member ();

Implementation(file, kvfile) = {
  ._ctor  = _ctor, 
  .read   = read,
  .write  = write,
  .lseek  = lseek,
  ._dtor  = _dtor
};

static int _ctor Member (filesystem_t *fs, inode_t inode, int flags) {
  MemberOf(kvfile);

  this.fs = (kvfs_t *)fs;
  base.inode = inode;
  base.refcnt = 0;
  base.ffset = 0;
  base.flags = flags;
  return 0;
}

static ssize_t read Member (char *buf, size_t size) {
  MemberOf(kvfile);

  struct kvfs_kvp *kvp = &(this.fs->kvp[base.inode]);
  if (base.offset + size > base.length) {
    size = kvp->length - base.offset;
    memcpy(buf, kvp->data, size);
    base.offset = kvp->length;
    return 0;
  } else {
    memcpy(buf, kvp->data, size);
    base.offset += size;
    return size;
  }
}

static ssize_t write Member (const char *buf, size_t size) {
  MemberOf(kvfile);

  struct kvfs_kvp *kvp = &(this.fs->kvp[base.inode]);
  if (base.offset + size > kvp->capacity) {
    int newcap = ((base.offset + size) * 2);
    char *newdata = pmm->alloc(newcap);
    if (newdata == NULL) {
_debug("Failed to allocate memory!");
      return -1;
    }
    memcpy(newdata, kvp->data, kvp->length);
    pmm->free(kvp->data);
    kvp->data = newdata;
    kvp->capacity = newcap;
  }
  memcpy(kvp->data + base.offset, buf, size);
  base.offset += size;
  if (base.offset > kvp->length)
    kvp->length = base.offset;
  return size;  
}

static int lseek Member (off_t offset, int whence) {
  MemberOf(kvfile);

  struct kvfs_kvp *kvp = &(this.fs->kvp[base.inode]);
  switch (whence) {
    case SEEK_SET:
      break;
    case SEEK_CUR:
      offset += base.offset; 
      break;
    case SEEK_END:
      offset += kvp->length;
      break;
    default:
_debug("Invalid whence paramenter (%d)", whence);
      return -1;
  }
  if (offset < 0 || offset >= kvp->length) {
_debug("Offset out of range!");
    return -1;
  }
  base.offset = offset;
  return offset;
}

static void _dtor() {
  MemberOf(kvfile);

  assert(base.refcnt == 0);
  return;
} 

