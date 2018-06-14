#ifndef __FS__DEVFS_H__
#define __FS__DEVFS_H__

#include <kernel.h>
#include <pmr.h>

#define MAX_DEV   16

typedef struct devfs {
  Inherits(filesystem);

  struct device {
    int valid;
    char name[32];
    int mode;
    int (*getch)();
    int (*putch)(int ch);
  } devices[MAX_DEV];

} devfs_t;

extern Implementation(filesystem, devfs);

typedef struct devfile {
  Inherits(file);

} devfile_t;

extern Implementation(file, devfile);

#endif 
