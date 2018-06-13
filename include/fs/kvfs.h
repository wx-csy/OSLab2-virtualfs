#ifndef __FS__KVFS_H__
#define __FS__KVFS_H__

#include <kernel.h>
#include <pmr.h>

#define MAX_KVP   128

typedef struct kvfs {
  struct filesystem base;

  struct kvfs_kvp {
    int valid;
    char key[32];
    int length, capacity;
    char *data;
  } kvp[MAX_KVP];

} kvfs_t;

extern Implementation(filesystem, kvfs);

typedef struct kvfile {
  struct file base;
} kvfile_t;

extern Implementation(file, kvfile_t);

#endif 
