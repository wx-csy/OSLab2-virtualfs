#ifndef __FS__DEVFS_H__
#define __FS__DEVFS_H__

#include <kernel.h>
#include <pmr.h>

typedef struct devfs {
  struct filesystem base;

  struct {
    char name[32];
    int mode;
    file_t *(*open)(int flags);
  } devices[16];

} devfs_t;

extern Implementation(filesystem, devfs);

#endif 
