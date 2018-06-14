#include <pmr.h>
#include <fs/devfs.h>
#include <fs/devices.h>
#include <string.h>
#include <assert.h>

#define DEBUG_ME
#include <debug.h>

static int      _ctor   Member (const char *name);
static inode_t  lookup  Member (const char *path);
static inode_t  create  Member (const char *path);
static int      access  Member (inode_t inode, int mode);
static file_t*  open    Member (inode_t inode, int flags);
static void     _dtor   Member ();

Implementation(filesystem, devfs) = {
  ._ctor  = _ctor, 
  .lookup = lookup,
  .create = create,
  .access = access,
  .open   = open,
  ._dtor  = _dtor
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

static int _ctor Member (const char *name) {
  MemberOf(devfs);
  
  strncpy(base.name, name, sizeof base.name);
  base.name[(sizeof base.name) - 1] = 0;
  base.refcnt = 0; 

  for (int i = 0; i < MAX_DEV; i++) {
    this.devices[i].valid = 0;
  }

  load_device(&(this.devices[0]), "null", dev_null_getch, dev_null_putch);
  load_device(&(this.devices[1]), "zero", dev_zero_getch, dev_null_putch);
  load_device(&(this.devices[2]), "random", dev_random_getch, NULL);
  return 0;  
}

static inode_t lookup Member (const char *path) {
  MemberOf(devfs);

  for (int i = 0; i < MAX_DEV; i++) {
    if (!this.devices[i].valid) continue;
    if (strcmp(path, this.devices[i].name)) continue;
    return i;
  }
_debug("Device not found!");
  return -1;
}

static inode_t create Member (const char *path) {
  MemberOf(devfs);

  // this file system does not support creating new files
  return -1;
}

static int access Member (inode_t inode, int mode) {
  MemberOf(devfs);

  return mode == (this.devices[inode].mode & mode) ? 0 : -1;
}

static file_t *open Member (inode_t inode, int flags) {
  MemberOf(devfs);
  
  file_t *file = New(devfile, (void *)&this, inode, flags);
  if (file != NULL) file->refcnt++;
  return file;
}

static void _dtor Member () {
  MemberOf(devfs);

  assert(base.refcnt == 0);
}

