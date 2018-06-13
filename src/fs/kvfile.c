#include <kernel.h>
#include <fs/kvfs.h>

// #define DEBUG_ME
#include <debug.h>

static int _ctor(file_t *file, filesystem_t *fs, inode_t inode, int flags);
static ssize_t read(file_t *file, char *buf, size_t size);
static ssize_t write(file_t *file, const char *buf, size_t size);
static off_t lseek(file_t *file, off_t offset, int whence);
static void _dtor(file_t *file);

Implementation(file, kvfile) = {
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
  struct kvfs_kvp *kvp = &(((kvfs_t *)(file->fs))->kvp[file->inode]);
  if (file->offset + size > kvp->length) {
    size = kvp->length - file->offset;
    memcpy(buf, kvp->data, size);
    kvp->offset = kvp->length;
    return 0;
  } else {
    memcpy(buf, kvp->data, size);
    kvp->offset += size;
    return size;
  }
}

static ssize_t write(file_t *file, const char *buf, size_t size) {
  struct kvfs_kvp *kvp = &(((kvfs_t *)(file->fs))->kvp[file->inode]);
  if (file->offset + size > kvp->capacity) {
    int newcap = ((file->offset + size) * 2);
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
  memcpy(kvp->data + file->offset, buf, size);
  file->offset += size;
  if (file->offset > kvp->length)
    kvp->length = file->offset;
  return size;  
}

static int lseek(file_t *file, off_t offset, int whence) {
  switch (whence) {
    case SEEK_SET:
      break;
    case SEEK_CUR:
      offset += file.offset; 
      break;
    case SEEK_END:
      offset += file.length;
      break;
    default:
_debug("Invalid whence paramenter (%d)", whence);
      return -1;
  }
  if (offset < 0 || offset >= file.length) {
_debug("Offset out of range!");
    return -1;
  }
  file.offset = offset;
  return offset;
}

static void _dtor(file_t *file) {
  return;
} 

