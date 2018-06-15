#include <pmr.h>
#include <fs/devfs.h>
#include <fs/devices.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define DEBUG_ME
#include <debug.h>

static int      _ctor   Member (const char *name);
static int      walk    Member 
    (int (*fn)(const char *path, inode_t inode, int mode, int length));
static inode_t  lookup  Member (const char *path);
static inode_t  create  Member (const char *path);
static int      access  Member (inode_t inode, int mode);
static file_t*  open    Member (inode_t inode, int flags);
static void     _dtor   Member ();

Implementation(filesystem, devfs) = {
  ._ctor  = _ctor,
  .walk   = walk, 
  .lookup = lookup,
  .create = create,
  .access = access,
  .open   = open,
  ._dtor  = _dtor
};

static void load_device(struct device *dev, const char *name, 
    int (*getch)(), int (*putch)(int ch)) {
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
_debug("Initializing \"%s\" ...", base.name);
  for (int i = 0; i < MAX_DEV; i++) {
    this.devices[i].valid = 0;
  }

  srand(time(NULL));

  load_device(&(this.devices[0]), "null", dev_null_getch, dev_null_putch);
  load_device(&(this.devices[1]), "zero", dev_zero_getch, dev_null_putch);
  load_device(&(this.devices[2]), "random", dev_random_getch, NULL);
  load_device(&(this.devices[3]), "stdin", dev_stdin_getch, NULL);
  load_device(&(this.devices[4]), "stdout", NULL, dev_stdout_putch);
  return 0;  
}

static int walk Member 
    (int (*fn)(const char *path, inode_t inode, int mode, int length)) {
  MemberOf(devfs);

  for (int i = 0; i < MAX_DEV; i++) {
    if (this.devices[i].valid) {
      int val = fn(this.devices[i].name, i, this.devices[i].mode, 0);
      if (val != 0) return val;
    }
  }
  return 0;
}

static inode_t lookup Member (const char *path) {
_debug("devfs.lookup path=%s", path);
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

  if (inode < 0 || inode >= MAX_DEV) return -2;
  if (!this.devices[inode].valid) return -2;
  return mode == (this.devices[inode].mode & mode) ? 0 : -1;
}

static file_t *open Member (inode_t inode, int flags) {
_debug("devfs.open inode=%d, flags=%d", inode, flags);
  MemberOf(devfs);
  
  if (inode < 0 || inode >= MAX_DEV) return NULL;
  if (!this.devices[inode].valid) return NULL;
  file_t *file = New(devfile, (void *)&this, inode, flags);
  if (file != NULL) file->refcnt++;
  return file;
}

static void _dtor Member () {
  MemberOf(devfs);

  assert(base.refcnt == 0);
}

