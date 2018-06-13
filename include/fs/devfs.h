#ifndef __FS__DEVFS_H__
#define __FS__DEVFS_H__

#include <kernel.h>
#include <pmr.h>

#define MAX_DEV   16

typedef struct devfs {
  struct filesystem base;

  struct {
    int valid;
    char name[32];
    int mode;
    file_t *(*open)(int flags);
  } devices[MAX_DEV];

} devfs_t;

extern Implementation(filesystem, devfs);

#endif 
