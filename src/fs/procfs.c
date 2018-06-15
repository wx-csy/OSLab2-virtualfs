#include <pmr.h>
#include <fs/procfs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

static int      _ctor   Member (const char *name);
static int      walk    Member 
    (int (*fn)(const char *path, inode_t inode, int mode, int length));
static inode_t  lookup  Member (const char *path);
static inode_t  create  Member (const char *path);
static int      access  Member (inode_t inode, int mode);
static file_t*  open    Member (inode_t inode, int flags);
static void     _dtor   Member ();

Implementation(filesystem, procfs) = {
  ._ctor  = _ctor,
  .walk   = walk, 
  .lookup = lookup,
  .create = create,
  .access = access,
  .open   = open,
  ._dtor  = _dtor
};

static int _ctor Member (const char *name) {
  MemberOf(procfs);
  
  strncpy(base.name, name, sizeof base.name);
  base.name[(sizeof base.name) - 1] = 0;
  base.refcnt = 0; 
_debug("Initializing \"%s\" ...", base.name);
  return 0;  
}

#define CPUINFO_INODE 128
#define MEMINFO_INODE 129

static int walk Member 
    (int (*fn)(const char *path, inode_t inode, int mode, int length)) {
  MemberOf(procfs);

  for (int i = 0; i < NR_THREADS; i++) {
    char buf[32];
    if (threads[i] != NULL) {
      sprintf(buf, "%d", i);
      int val = fn(buf, i, R_OK, 0);
      if (val != 0) return val;
    } 
  }
  
  int val;
  val = fn("cpuinfo", CPUINFO_INODE, R_OK, 0);
  if (val != 0) return val;
  val = fn("meminfo", MEMINFO_INODE, R_OK, 0);
  if (val != 0) return val;

  return 0;
}

static inode_t lookup Member (const char *path) {
  MemberOf(procfs);

  if (strcmp(path, "cpuinfo") == 0) return CPUINFO_INODE;
  if (strcmp(path, "meminfo") == 0) return MEMINFO_INODE;
  int tid = atoi(path);
  if (tid < 0 || tid >= NR_THREADS) return -1;
  if (threads[tid] == NULL) return -1;
  return tid;
}

static inode_t create Member (const char *path) {
  MemberOf(procfs);
  
  // this file system does not support creating new files
  return -1;
}

static int access Member (inode_t inode, int mode) {
  MemberOf(procfs);

  return mode == (R_OK & mode) ? 0 : -1;
}

static file_t *open Member (inode_t inode, int flags) {
  MemberOf(procfs);

  char *buf;
  int size;
  otrofile_t *file;

  switch (inode) {
    case CPUINFO_INODE:
      buf = pmm->alloc(512);
      size = sprintf(buf, "CPU Type: Unknown\nTime: %s\n", 
          ctime(time(NULL)));
      file = New(otrofile, (void *)&this, inode, flags);
      file->length = size;
      file->data = file;
      return file;
    case MEMINFO_INODE:
      buf = pmm->alloc(512);
      size = sprintf(buf, "Memory Type: Unknown\n", asctime());
      file = New(otrofile, (void *)&this, inode, flags);
      file->length = size;
      file->data = file;
      return file;
    default:
      return NULL;
  }
/*  
  file_t *file = New(devfile, (void *)&this, inode, flags);
  if (file != NULL) file->refcnt++;

  return file;
*/
  return NULL;
}

static void _dtor Member () {
  MemberOf(procfs);

  assert(base.refcnt == 0);
}

